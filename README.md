# hw4
*****parking:*****\
一開始使用xbee傳送rpc指令給bbcar，bbcar接收到指令後呼叫rpcfunction，首先依照E(east)/W(west)判斷車子的方向，接著用d2計算出要退後多少，
然後向左或向右轉彎90度(依據E/W方向)，再依據d1計算出要退後多少距離，完成停車。\
*****line following:*****\
首先再openmv的main.py中增加判斷為直線的條件，進而將環境中多餘的線段消除，如此範圍內會只剩下需要被偵測的那條直線，讓車子沿著這條直線行走，當線段結束，便讓車子停下來不再前進。\
*****position calibration:*****\
使用openmv偵測apriltag，並記錄其偵測到的各項距離、角度，將需要的資料擷取下來並回傳給mbed，將資料進行運算過後得到bbcar需要位移的距離，並將車子移動到新地點，使得其正面對apriltag，\
最後則用ping來偵測車子與apriltag之間的距離，並利用xbee將此數據回傳到pc端以供確認。