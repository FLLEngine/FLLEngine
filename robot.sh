#!/usr/bin/env bash

while (( $# ))
do
    case "$1" in

        --update | -u)
		docker exec --tty fllngine_armel make
		rsync -avz --del --exclude '.*' /home/mindbenders/MBRobot/$3/* /home/mindbenders/MBRobot/$3staging/
		cp /home/mindbenders/MBRobot/FLLEngine/micropython/ports/unix/micropython /home/mindbenders/MBRobot/$3staging/fllengine-micropython
		echo "test"
		expect -c "
		spawn rsync -avz --del /home/mindbenders/MBRobot/$3staging robot@10.42.0.$2:/home/robot/MBRobot/
		expect \"(robot@10.42.0.$2) Password: \" {
                	send \"maker\n\"
		}
		interact
		";

		exit 0
	;;

        --run | -r)
		expect -c "
		spawn ssh robot@10.42.0.$2 \"brickrun /home/robot/MBRobot/$3staging/fllengine-micropython /home/robot/MBRobot/$3staging/main.py\"
		expect \"(robot@10.42.0.$2) Password: \" {
			send \"maker\n\"
		}
		interact
		";
		exit 0
	;;

	--both | -b)
		docker exec --tty fllngine_armel make
		rsync -avz --del --exclude '.*' /home/mindbenders/MBRobot/$3/* /home/mindbenders/MBRobot/$3staging/
		cp /home/mindbenders/MBRobot/FLLEngine/micropython/ports/unix/micropython /home/mindbenders/MBRobot/$3staging/fllengine-micropython
		expect -c "
		spawn rsync -avz --del /home/mindbenders/MBRobot/$3staging robot@10.42.0.$2:/home/robot/MBRobot/
		expect \"(robot@10.42.0.$2) Password: \" {
                	send \"maker\n\"
		}
		interact
		";
		expect -c "
		spawn ssh robot@10.42.0.$2 \"brickrun /home/robot/MBRobot/$3staging/fllengine-micropython /home/robot/MBRobot/$3staging/main.py\"
		expect \"(robot@10.42.0.$2) Password: \" {
			send \"maker\n\"
		}
		interact
		";
		exit 0
	;;

# more code...

    esac
done
