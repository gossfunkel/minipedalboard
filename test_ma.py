from direct.showbase.ShowBase import ShowBase

if __name__ == '__main__':
    ShowBase()
    tt = loader.loadSfx("test_tone_2.wav")
    tt.setLoopCount(0)
    tt.setLoop(True)
    tt.play()
    base.accept("escape", base.user_exit)

    print(base.sfxManagerList[0])

    base.run()