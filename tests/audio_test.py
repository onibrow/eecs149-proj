import vlc
import time
song = "akubi.mp3"
p = vlc.MediaPlayer(song)
p.play()
t = 120
i = 0
print("Playing {}".format(song))
while (i < t):
    print(i)
    time.sleep(1)
    i += 1
p.stop()
