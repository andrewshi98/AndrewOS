sudo apt-get install  openssh-server
# Open /etc/ssh/sshd_config and append the following:
# Change - PermitRootLogin no
# Add - AllowUsers yourusername
# Change - PasswordAuthentication yes
# Add - UsePrivilegeSeparation no
# Change - ListenAddress 0.0.0.0
# Change - Port 2200
# Then do sudo service ssh --full-restart
