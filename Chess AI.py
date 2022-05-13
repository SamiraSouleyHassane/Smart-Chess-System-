#Author Samira Souley Hassane
#Version 1.0 May 12th 2022
#This code takes serial input corresponding to UCI chess moves and 
#allows to play chess against Stockfish chess AI engine 

from prompt_toolkit import print_formatted_text as print
from prompt_toolkit.styles import Style
from prompt_toolkit.formatted_text import FormattedText

import chess
import chess.engine

import os
from time import sleep

import serial 

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD)

#LCD 
from lcd_api import LcdApi
from i2c_lcd import I2cLcd
 
I2C_ADDR = 0x20
I2C_NUM_ROWS = 2
I2C_NUM_COLS = 16
 
lcd = I2cLcd(1, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)

#Setting pins
red_button    = 16
blue_button   = 18
green_button  = 22
yellow_button = 11
black_button  = 13

#Buttons are active low
GPIO.setup(red_button, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(green_button, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(blue_button, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(yellow_button, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(black_button, GPIO.IN, pull_up_down = GPIO.PUD_UP)

#Global variables
chess_position = [
    "a8", "a7", "a6", "a5", "a4", "a3", "a2", "a1",
    "b8", "b7", "b6", "b5", "b4", "b3", "b2", "b1",
    "c8", "c7", "c6", "c5", "c4", "c3", "c2", "c1",
    "d8", "d7", "d6", "d5", "d4", "d3", "d2", "d1",
    "e8", "e7", "e6", "e5", "e4", "e3", "e2", "e1",
    "f8", "f7", "f6", "f5", "f4", "f3", "f2", "f1",
    "g8", "g7", "g6", "g5", "g4", "g3", "g2", "g1",
    "h8", "h7", "h6", "h5", "h4", "h3", "h2", "h1"]

#Declare 4 lists
initial_values = []
first_values = []
second_values = []
wrong_position = []
illegal_moves = []


j = 0
num_push = 0
castling = 0

#Establish serial communication with the arduino 
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.reset_input_buffer()
ser.flush()

#define Chess board and AI 
engine = chess.engine.SimpleEngine.popen_uci('/usr/games/stockfish')
board = chess.Board()

#DEfintions of functions
def checkInitialPositions():
    final_check = True

    check_list = [
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False,
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False, 
    False, False, False, False, False, False, False, False]

    robot_piece = 0
    no_piece1   = 2
    no_piece2   = 4
    human_piece = 6

    for i in range(0,2):
        for ii in range (0,8):
            global initial_values
            if(initial_values[robot_piece] > 370 and initial_values[robot_piece] < 600):
                check_list[robot_piece] = True
            if(initial_values[human_piece] > -1 and initial_values[human_piece]  < 70):
                check_list[human_piece] = True
            if(initial_values[no_piece1] > 180 and initial_values[no_piece1]    < 300):
                check_list[no_piece1] = True
            if(initial_values[no_piece2] > 180 and initial_values[no_piece2]    < 300):
                check_list[no_piece2] = True

            robot_piece = robot_piece + 8
            human_piece = human_piece + 8
            no_piece1   = no_piece1 + 8
            no_piece2   = no_piece2 + 8

        robot_piece = 1
        no_piece1   = 3
        no_piece2   = 5
        human_piece = 7

    #Keeping track of wrong positions
    wrong_position.clear()
    for element in range(0,64):
        final_check = final_check and check_list[element]
        if check_list[element] == False:
            wrong_position.append(chess_position[element])

    #Returning true or false
    if final_check:
        return True
    else:
        return False        

def display_two(line1, line2):
    lcd.clear()
    lcd.move_to(0,0)
    lcd.putstr(line1)
    lcd.move_to(0,1)
    lcd.putstr(line2)

def display(line1):
    lcd.clear()
    lcd.move_to(0,0)
    lcd.putstr(line1)
 
def illegal_move():
    global illegal_moves
    
    while len(illegal_moves) > 0 and len(illegal_moves) < 3:
        last_move = illegal_moves[-1]
        
        if ser.in_waiting > 0:
            move = ser.readline().decode('utf-8').rstrip() 
            
            if(last_move != move and len(move) == 4):
                first_position = move[0:2]
                second_position = move[2:4]
                reverse_transition = second_position + first_position
                illegal_moves.append(reverse_transition)
            
            elif(last_move == move):
                illegal_moves.pop(-1)
                display("Reverse done")
                print("Reverse done")
            
            if len(illegal_moves) > 0:
                display_two("Reverse last move: ", illegal_moves[-1])
                print("Reverse last move: " + illegal_moves[-1])
                
            elif len(illegal_moves) == 0:
                display_two("You can make", "a new move")
                print("You can make a new move \n")
               
      
                
    if len(illegal_moves) == 3:
        display_two("3 Moves not","reversed")
        print("3 Moves not reversed \n")

        sleep(3)
        display("Game Ended")
        print("Game Ended. \n")       
        
    if len(illegal_moves) == 0:
        return False 
    else:
        return True
 
def main():
    
    #define a function to play a game
    def play_game():
        output = board.unicode(invert_color= False, borders= True, empty_square = ' ') + "\n \n Press the blue button to start the game..."
        
        text = FormattedText([('#0000FF bg:#FFFFFF',output), ])
        print(text)
        
        display_two("Press blue to","start the game")
        
            
        blue_state = GPIO.input(blue_button)
        while(not blue_state):
            blue_state = GPIO.input(blue_button)  
            sleep(0.5)
            
        display("Select AI level")
        print("Select AI level \n")
                
        first_time = 0
        
        
        #User selects the level 
        level = 0                 #default AI level 
        blue_state = GPIO.input(blue_button)
        while not blue_state and level < 2:     #check if the blue button has been pressed
            yellow_state = GPIO.input(yellow_button)
            if yellow_state:
                level +=1
            sleep(0.4)
            blue_state = GPIO.input(blue_button)

        engine.configure({"Skill Level": level*5})  #set the AI level
        
        print("level: "+str(level))
        #print the level selected 
        if(level == 0):
            print("You selected Beginner level \n")
            display_two("AI level:","Beginner")
            sleep(1)
        elif(level == 1):
            print("You selected Intermediate level \n")
            display_two("AI level:","Intermediate")
            sleep(1)
        elif (level==2):
            print("You selected Advanced level \n")
            display_two("AI level:"," Advanced")
            sleep(1)
    
            
        display("Game started...")
        
        os.system('clear')
        output = board.unicode(invert_color= False, borders= True, empty_square = ' ') + "\n \n Game started..."
        text = FormattedText([('#0000FF bg:#FFFFFF',output), ])
        print(text)

        red_state = GPIO.input(red_button)             #button to end the game 
        end_game_check = False                         #boolean to end the game when 3 consecutive illegal move have been made

        while not board.is_game_over() and not red_state:
            legal_moves = list(board.legal_moves)       #create a list of legal moves
            index = 0
            length = len(legal_moves)

            print("Press green button to make a move or black button to display a legal move")
            
            if(first_time == 0):
                first_time = 1
                display_two("Green to move", "Black for hint")
                
            green_state = GPIO.input(green_button)
            while(not green_state):
                black_state = GPIO.input(black_button)
                if black_state:
                    print("A legal move you can make: "+ legal_moves[(index%length)].uci())

                    display_two("A legal move you", "can make: "+ legal_moves[(index%length)].uci())
                    
                    index +=1 
                    
                sleep(0.1)
                green_state = GPIO.input(green_button)
            
            print("Press green when done")
            display_two("Press green","when done")
            
            while(ser.in_waiting == 0):
                pass
            
            if ser.in_waiting > 0:
                player_move = ser.readline().decode('utf-8').rstrip()     
                print("Player move:", player_move)
                 
                if player_move == "End":    #check if red button has been pressed instead of making a move 
                    end_game_check = True   #Game has been ended because the red button has been pressed 
                    break                   #exit to end the game 
                
                else: 
                    if (len(player_move) > 4 or len(player_move) < 3) :
                        print("Invalid transition")
                        display("Invalid move")
                        
                    else:
                        move = player_move
                        player_move = chess.Move.from_uci(player_move)
                        
                        if player_move in board.legal_moves:            #check if it is a legal move
                            board.push(player_move)
                            result = engine.play(board, chess.engine.Limit(time=1))
                            board.push(result.move)
                                
                            AI_move = result.move.uci()                  #get a UCI string for the AI's move
                            display_two("Your move: " + move, " AI move: "+ AI_move )
                                    
                            output = board.unicode(invert_color= False, borders= True, empty_square = ' ')
                            os.system('clear')
                                
                            output = output + "\n \n \n The AI move was: "+ AI_move + " \n Your move was: " + player_move.uci()
                            text = FormattedText([
                                ('#0000FF bg:#FFFFFF',output),
                        
                            ])
                            print(text)

                        else:    #it is an illegal move 
                            display("Illegal move")               #send illegal move to the arduino
                            sleep(1)
                            
                            first_position = move[0:2]
                            second_position = move[2:4]
                            reverse_transition = second_position+first_position
                            illegal_moves.append(reverse_transition)
                            
                            display_two("Reverse Last", "Move " + reverse_transition)
            
                            os.system('clear')
                            
                            output = board.unicode(invert_color= False, borders= True, empty_square = ' ') + "\n \n This is an illegal move. \n Your move was: "+ player_move.uci() + " \n" + "Reverse Last Move: " + reverse_transition
                            text = FormattedText([
                                ('#0000FF bg:#FFFFFF',output),
                        
                            ])
                            print(text)
                            
                            end_game_check = illegal_move()
                            
                            if(end_game_check):         # check if the player has not undone 3 consecutives illegal moves 
                                break                   # if true end the game
                        #sleep(5)
                        
                red_state = GPIO.input(red_button)      #check if the red button has been pressed 

        #Game has ended    
        if red_state or end_game_check:   #game has ended because the red button has been pressed 
            board.reset()                 # reset the board 
            display("Game has been ended")
            print("Game has been ended")
        
        else:                              #game has ended because there is a winner 
            outcome = board.outcome()      #find the outcome of the game 
            
            #Find the winner of the game 
            if outcome.winner == chess.WHITE:
                winner = "Game over. You won!"
                display_two("Game is over.", "You won!")

            elif outcome.winner == chess.BLACK:
                winner = "Game is over. Stockfish won"
                display_two("Game is over.", "Stockfish won")
            
            else:
                winner = "Game is over. It is a draw"
                display_two("Game is over.", "It is a draw")

            #Print the winner of the game
            os.system('clear')
            output = board.unicode(invert_color= False, borders= True, empty_square = ' ') + "\n \n " + winner 
            text = FormattedText([
                        ('#0000FF bg:#FFFFFF',output),
            ])
            
            #Display the winner
            print(text)
        
        sleep(1)
        ser.write(b"Game over\n")    #send message to arduino to restart game 

        blue_state = GPIO.input(blue_button)
        while not blue_state:
            blue_state = GPIO.input(blue_button)
        
        #reset serial
        ser.reset_input_buffer()
        ser.flush()
        sleep(1)
        
        #reset castling
        global castling
        castling = 0
        
        #recursively call play_game to play a new game once the blue button has been pressed again
        play_game()

    #play a game 
    play_game()

    
if __name__ == "__main__":
    main()





