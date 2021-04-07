import pyodbc
import os
import sys
import pandas as pd
import pprint
import tempfile
import datetime
import subprocess

class YbConnector:
    """ A pytezza-esque connector to Yellowbrick. """
    def __init__(self,host,db,user,pw):
        """ """
        self.host = host
        self.db = db
        self.user = user
        self.pw = pw
        self.dry_run_mode = False
        self.verbosity_level = 0
        
        self.connstring="Driver={PostgreSQL Unicode(x64)};Server=%s;Port=5432;Database=%s;Uid=%s;Pwd=%s;"%(host,db,user,pw)
        self.connection=None
        self.connection=self.getConnection()
        
    def __del__(self):
        if self.connection:
            self.disconnect()
        return

    def runDry(self):
        """ """
        self.dry_run_mode=True
    
    def runLive(self):
        """ """
        self.dry_run_mode=False
    
    def setVerbosityLevel(self,vl):
        """ 0 for quiet; 1 for loud """
        if vl in (0,1):
            self.verbosity_level=vl
        else:
            self.verbosity_level=0
    
    def getConnection(self):
        """ """
        try:
            return pyodbc.connect(self.connstring,autocommit=True)
        except Exception as details:
            print("Error connecting using %s"%(self.connstring))
            print(">>> ",details)
            sys.exit()

    def disconnect(self):
        """ """
        if self.connection:
            self.connection.close()
        return
    
    def vprint(self,txt,verbose=True):
        """ """
        if verbose:
            print(txt)
        return
    
    def qdrop(self,table):
        """ """
        self.ybPassthrough("""drop table if exists {table};""".format(table=table))
    
    def tableExists(self,table):
        """ """
        return self.ybPassthrough("""
            select count(*) as n
            from information_schema.tables
            where upper(table_schema)=upper('{db}')
              and upper(table_name)=upper('{table}')
        """.format(db=self.db,table=table),withResults=True).N[0]>0
    
    def tableN(self,table):
        """ """
        return self.ybPassthrough("""
            select count(*) as n
            from {table}
        """.format(db=self.db,table=table),withResults=True).N[0]
    
    def ybPassthrough(self,q,withResults=False,printQuery=False,dryRun=False,ignoreErrors=False):
        """ Run a query.  If a resultset is expected, return it as a pandas dataframe.
            if printQuery is true, print the first 50 characters of the query.
            If dryRun is true, print the whole query, but do not run it.
        """
        if (printQuery or self.verbosity_level==1):
            print("ybPassthrough:\n%s\n%s\n%s\n" %(80*"=",q,80*"="))
        if (dryRun or self.dry_run_mode):
            print("ybPassthrough would have run this:\n%s\n\n" %q)
            return None
        
        if self.connection:
            if withResults:
                return pd.read_sql(q, self.connection)
            else:
                try:
                    rs = self.connection.execute(q)
                    self.connection.commit()
                except Exception as details:
                    print("Problem with your query.  Error first, then query below:\n{the_details}\n\n{the_query}".format(the_details=details,the_query=q))
                    if not ignoreErrors:
                        print("Exiting due to query problem.")
                        sys.exit(1)
                    else:
                        print("Ignoring query problem.  You might want to look at that.")
                        pass
                return
        else:
            print("You're not connected, and I'm not going to connect for you.  Fix your application.")
            sys.exit(1)
        return
    qq = ybPassthrough
    
    def dfToYb(self,df_src,db,tablename,distribute_on="random",clobber=False,ybTypes=(),verbose=False,ignoreLoadErrors=False):
        """ Given a pandas df, push that table to yellowbrick, optionally replace the existing table in yellowbrick.
            If ybTypes immutable is provided, use those types and to hell with the consequences.
            Otherwise, infer the types.
        """
        
        fqtable = "%s.%s"%(db,tablename)
        print("Loading %d rows and %d columns into %s."%(df_src.shape[0],df_src.shape[1],fqtable))
        self.vprint("Here is a sample of the source data:\n%s\n"%df_src.head(5),verbose)
        
        if not ybTypes:
            #If were weren't supplied types, guess them ourselves.
            ybTypes=self.getYbTypesFromDf(df_src.dtypes)
        
        if clobber:
            self.vprint("Dropping %s." %fqtable,verbose)
            self.ybPassthrough("drop table if exists %s;"%fqtable)
        
        maxbad="0"
        if ignoreLoadErrors:
            maxbad="-1"
        
        ddl = ""
        for i in range(len(df_src.columns)):
            #New DDL line for each element.  Trickiness to handle no-comma for last element.
            ddl+="    %s %s%s\n"%(df_src.columns[i],ybTypes[i],(i<len(df_src.columns)-1)*",")
        
        dump=tempfile.mkstemp(".csv")[1]
        self.vprint("Dumping local data to %s." %dump,verbose)
        df_src.to_csv(dump,index=False,header=True,line_terminator="\n")
        self.vprint("Dumped %0.2fMB."%(os.path.getsize(dump)/1024/1024),verbose)
        
        self.qq("""
            create table {fqtable} (
                {ddl}
            )
            ;
        """.format(fqtable=fqtable,ddl=ddl),printQuery=True)
        
        os.environ["YBPASSWORD"]=self.pw
        cmd="""ybload --host {pb.host} --username {pb.user} --dbname {pb.db} --max-bad-rows {maxbad} --csv-skip-header-line true --table {target} {src}"""
        cmd=cmd.format(pb=self,maxbad=maxbad,target=fqtable,src=dump)
        print("Running this now ===>",cmd)
        
        try:
            print(subprocess.call(cmd))
        except Exception as details:
            print("There was a problem running ybload.  Make sure it's installed, is on your PATH, and that Java is installed:\n{the_details}\n".format(the_details=details))
            sys.exit(1)
        
        lrows = self.qq("select count(*) as n from %s" %fqtable,withResults=True).n[0]
        if len(df_src)==lrows:
            print("Looks good: We dumped %d rows, and then loaded %d rows." %(len(df_src),lrows))
        else:
            print("THERE MIGHT HAVE BEEN A PROBLEM LOADING!  Check the output.")
        
        return
        
    def ybToDf(self,db,table,where="",order=""):
        """ """
        q = "select * from %s.%s"%(db,table)
        if where:
            q+="\nwhere %s" %where
        if order:
            q+="\norder by %s" %order
        return self.ybPassthrough(q,withResults=True)
        
    def getYbTypesFromDf(self,dftypes):
        """ Try to map.  This is almost vulgar.
            Return a tuple the same size as dftypes."""
        default_yb = "varchar(1024)"
        df2yb = {'int64':'bigint',
                 'int32':'bigint',
                 'float64':'float',
                 'float32':'float',
                 'object':'varchar(1024)'}
        
        ybtypes = []
        for t in dftypes:
            ybtypes.append(df2yb.get(t.name,default_yb))
        return(tuple(ybtypes))
        
    def ybDumpToDisk(self,db,table,outfile,delimiter=",",writeHeader=True,order_by=None,compress=False,chunk_size=500000):
        """ """
        if compress:
            print("Compress option is not yet supported.")
        if order_by:
            order_by = "order by "+order_by
        
        print("Dumping %s to %s..." %(table,outfile))
        q="select * from {db}.{table} {order_by}".format(db=db,outfile=outfile,delimiter=delimiter,table=table,order_by=order_by)
        
        chunks_used=0
        starttime=datetime.datetime.now()
        with open(outfile,"w+") as h:
            for chunk in pd.read_sql_query(q,self.connection,chunksize=chunk_size):
                #Only write the header for the first chunk.
                if writeHeader and chunks_used>0:
                    writeHeader=False
                chunks_used+=1
                print("    Dumping chunk #%s"%chunks_used)
                h.write(chunk.to_csv(sep=delimiter,index=False,header=writeHeader,float_format='%.2f'))
        endtime=datetime.datetime.now()
        et=(endtime-starttime).total_seconds()
        print("Finished dumping to",outfile)
        print("Export took %0.2f seconds (%0.2f minutes)."%(et,et/60))
        return
        
def testYb():
    x = YbConnector("ybr-02.eyc.com","dbo","clevis","Chris1234!")

    # Run a query, and if there are results, pull them back as a Pandas DF and print them.
    my_results = x.ybPassthrough("select user,current_timestamp",withResults=True)
    pprint.pprint(my_results)

    # Run a query that doesn't send back a result set.
    x.ybPassthrough("""
        drop table if exists cjltest;
        create table cjltest as
            select '{value_for_my_field}' as my_field
                   ,current_timestamp as ts
        ;
    """.format(value_for_my_field="somevalue"))
    
    print(x.qq("select my_field from cjltest",withResults=True).my_field[0])
    
    nov=x.qq("select * from geadma100.lu_day where month_id=201811 order by d_date",withResults=True)
    x.dfToYb(nov,"public","cjlload",clobber=True,verbose=True)
    
if __name__ == "__main__":
    testYb()


