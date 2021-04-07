setup(name='pytezza',
      description='A python/netezza interface utility',
      long_description=long_description,
      version='0.1.0',
      url='https://github.com/massenz/filecrypt',
      author='Chris Levis',
      author_email='chris.levis@eyc.com',
      license='Apache2',
      classifiers=[
          'Development Status :: 4 - Beta',
          'Intended Audience :: System Administrators',
          'License :: OSI Approved :: Apache Software License',
          'Programming Language :: Python :: 3'
      ],
      packages=['pytezza'],
      install_requires=[
          'pandas>=0.23.0'
      ],
      entry_points={
          'console_scripts': [
              'encrypt=pytezza.main:run'
          ]
      }
)
