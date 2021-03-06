void haybail_activate()
{
/*to activate this you may also add the following line to stk-assets/farm/scene.xml (instead of creating it via the start script

<object type="action-trigger" action="haybail_activate" distance="30.0" xyz="100.72 10.20 -26.22" hpr="0.0 -0.0 0.0" scale="7.00 7.00 7.00"/>

*/
    displayMessage("Haybail deactivated");
    //disable("hayBail.b3d");
	TrackObject @t_obj = getTrackObject("hayBail.b3d");
     //t_obj.setEnable(false);
     PhysicalObject @haybail = t_obj.getPhysicalObject();
     haybail.disable();
     Mesh @haybailMesh = t_obj.getMesh();
     Animator @haybailAnimator = t_obj.getAnimator();
     haybailAnimator.setPaused(true);
     //if (haybail.isFlattener())squashKart(0,35.0);
     TrackObject @moo_sound = getTrackObject("cow");
     SoundEmitter @cowmoo = moo_sound.getSoundEmitter();
     cowmoo.stop();
}

void haybail_deactivate()
{
/*to activate this add the following line to stk-assets/farm/scene.xml

<object type="action-trigger" action="haybail-deactivate" distance="10.0" xyz="69.97 8.08 -107.84" hpr="0.0 -0.0 0.0" scale="7.00 7.00 7.00"/>

*/
     displayMessage("Haybail reactivated");
     //enable("hayBail.b3d");
     TrackObject @t_obj2 = getTrackObject("hayBail.b3d");
     //t_obj2.setEnable(false);
     PhysicalObject @haybail = t_obj2.getPhysicalObject();
     haybail.enable();

     squashKart(0,35.0); //id of kart,time to squash
     TrackObject @t_obj = getTrackObject("hayBail.b3d");
     Animator @haybailAnimator = t_obj.getAnimator();
     haybailAnimator.setPaused(false);
     TrackObject @moo_sound = getTrackObject("cow");
     SoundEmitter @cowmoo = moo_sound.getSoundEmitter();
     cowmoo.playOnce();
}


