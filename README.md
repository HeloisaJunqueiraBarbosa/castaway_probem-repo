# The Castaway Problem

The  Castaway  problem  consists  in  a  shark  that  pursues  a  castaway,  which  is on  an  island,  to  eat  him  when  he  reaches  the  border.  In  this  case  the  best strategy for the shark is to maintain the minimum distance from the castaway. The strategy presented here aims to make the castaway reach the edge before the shark reaches him.

## Instalation
To run the code using Fedora 
```bash
sudo dnf install https://repo.wr.sc.usp.br/fedora/warthog-robotics-repository-$(rpm -E %fedora).noarch.rpm
sudo dnf install GEARSystem-devel WRTools qt5-devel omniORB-devel
```

## Usage
After downloading it to local

1. Compile the program using
	 
        sh ./compile.sh
	
2. Execute the program using 5 diferents terminals:

  - First terminal run the commands
		 
        sudo systemctl start omniNames
		  
		WRBackbone
		
  - Second terminal cd to the folder of viewer/ and execute the command
		 
        ./viewer
		   
  - Third terminal cd to the folder of world/ and execute the command
		 
        ./world
		   
  - Fourth terminal cd to the folder of shark/ and execute the command
		 
        ./shark
		
  - Fifth terminal cd to the folder of human/ and execute the command
		 
        ./human
		
3. To stop the execution of the program press Ctrl+c in the viewer, world, shark and human terminal

4. To see the results run the code plot_results.m in MATLAB and change the function fopen() with the file path in your computer to the folder of world/
	 
    
