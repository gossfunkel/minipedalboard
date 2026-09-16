from direct.showbase.ShowBase import ShowBase
from direct.showbase import Audio3DManager
from panda3d.core import Vec3, CollisionTraverser
import numpy as np

if __name__ == '__main__':
    ShowBase()
    
    ball = loader.loadModel("models/smiley")
    ball.setPos(0.,1.,0.)

    ball_vel = Vec3(0., 1., 0.)

    audio3d = Audio3DManager.Audio3DManager(base.sfxManagerList[0], camera)

    tt = audio3d.loadSfx("test_tone_2.wav")
    audio3d.attachSoundToObject(tt, ball)
    #audio3d.setSoundVelocity(t, ball_vel)

    base.cTrav = CollisionTraverser()
    audio3d.setSoundVelocityAuto(tt)

    tt.setLoopCount(0)
    tt.setLoop(True)
    tt.play()
    base.accept("escape", base.user_exit)

    print(base.sfxManagerList[0])

    def update_ball(task):
        ball_vel = Vec3(np.sin(task.frame), np.cos(task.frame), 0.)
        ball.setPos(ball.getPos() + ball_vel)

    base.taskMgr.add(update_ball, "update_ball")

    base.run()