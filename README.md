# Smart-Chess-System-
This project allows a human to play chess against Stockfish AI chess engine using a physical chessboard. 
The project has three different components:
  - A smart chessboard that has hall effect sensors embedded to know where the human chess pieces are. Sensors readings are done using  Arduino Uno microcontroller. 
  - A roobtic arm to move chess pieces for the AI controlled by an Arduino Uno
  - A virtual chessboard run on Raspberry pi 4. 

A move on the chessboard is dectected by the Arduino Uno. This data is sent to the Raspberry pi using serial communication.
This project is developed by Samira Souley Hassane, Tobi Akerele and Miran Manasrah for engineering senior capstone project at TrinitY College. 
