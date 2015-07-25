#http://www.weewx.com/docs/setup.htm
# required packages:
sudo apt-get install python-configobj
sudo apt-get install python-cheetah
sudo apt-get install python-imaging
          
# required if hardware is serial or USB:
sudo apt-get install python-serial
sudo apt-get install python-usb
          
# required if using MySQL:
sudo apt-get install mysql-client
sudo apt-get install python-mysqldb

# required if you are on Raspbian and planning on using FTP:
sudo apt-get install ftp

# optional for extended almanac information:
sudo apt-get install python-dev
sudo apt-get install python-pip
sudo pip install pyephem
