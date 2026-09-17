from direct.showbase.ShowBase import ShowBase
from direct.showbase import Audio3DManager
from panda3d.core import Vec3, CollisionTraverser, load_prc_file_data
import numpy as np

load_prc_file_data('', "show-frame-rate-meter true")

RADIUS = 10.

if __name__ == '__main__':
    ShowBase()
    
    ball = loader.loadModel("models/smiley")
    ball.reparentTo(base.render)
    ball.setPos(0., RADIUS, 0.)

    ball_vel = Vec3(.04, 0., 0.)

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
        #ball_vel = Vec3(np.cos(task.frame/50.)*RADIUS, -np.sin(task.frame/50.)*RADIUS, 0.)
        #print(f"Ball velocity: {ball_vel}")
        
        #ball.setPos(Vec3(np.sin(task.frame/50.)*RADIUS, np.cos(task.frame/50.)*RADIUS, 0.))
        #ball.setPos(Vec3(np.sin(task.frame/50.)*3., RADIUS, 0.))
        ball_vel = Vec3(np.cos(task.frame/50.)*.04, 0., 0.)
        ball.setPos(ball.getPos() + ball_vel)
        return task.cont

    base.taskMgr.add(update_ball, "update_ball")

    base.run()