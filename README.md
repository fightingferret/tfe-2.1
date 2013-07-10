
The following file give some helpful hints on how to get started
running the TFE source code, along with a list of the changes made
so far.  We ask that you be patient with any errors that arrise
while opperating this source, and that you email us with any 
bugs that you discover.  Other then that, there are no
restrictions to what you can do with this source.  Use it, abuse it,
or throw it out .. its your perogative.

Due to the inefficiency of such an endeavor, notes will most likely cease
as the main way of announcing a new release.  A mailing list is being compiled
for this purpose.  If you would like to be added to this mailing list, simply
send me an email 'singed(at)beer.com' and let me know.  Enjoy :)

List of Bug Fixes/Changes
-------------------------

Thief and thief related items:
  -- Eye gouge, Garrote, Backstab, Steal, Sneak, Hide, Camouflage,
     Appraise, Disarm, Inspect, Pick Lock, Trip, Untrap
  -- Class Implemented

Fighting Related items:
  -- Bash, Berserk, Charge, Kick, Melee, Off-Hand Attack, Punch, Rescue,
     Spin Kick, Bite (for those lizardly guys)

-- Two handed weapons now take up two hands, and secondary weapons can be
   worn (if you are skilled enough to do it).

-- The Auction is fixed and enabled (NOTE: You can disable it if you decide
   you don't like the auction)

-- Light Levels are implemented.  A max of 5 balls of light has been placed
   on Create Light.  ATM there is no advantage to having more light, as
   long as it isn't dark.  Left room for the creation of a create dark 
   spell.

-- Compare now works with shop keepers again.

-- Bandage is implemented.

-- You can no longer move while seated (sorry .. no crab walking).

-- You can sit on objects again.

-- Socials and spell extras no longer have [BUG]'s in them.

-- Nifty little object triggers work (well .. as well as they did prewipe),
   and are currently be worked over in attempt to improve their functionality.
   You can pick berries and flowers again though :)

-- The Immortal Advance function works as well as I can get it for now.  When
   you advance a new immortal, they most logout and log back in.  The crashes
   should be fixed, etc.  If you run into any more problems with this, let me
   know.

-- Wimpies are fixed.  All of them work (given that you can use them of
   course).

-- Recall spell no longer hangs the mud.  Scrolls of Recall also work.


How To Get Your Immortals Rolling
---------------------------------

Included with this release is an ready made 99th level immortal character.
To make your own, simply raise your character to 99, then lower the temp
immortal to 1 and delete him.  The temp immortals stats are thus:

Name: TempImm
Password: llama

Here is a step by step list of what to do to make an immortal:
Login in as your new character (make account, etc.).
Login as Tempimm.
AS TEMPIMM: type 'advance <your characters name> 99'
AS YOURCHAR: type 'save' followed by 'quit'
Login as your character.
AS YOURCHAR: type 'advance tempimm 1' followed by 
 	          'force tempimm delete llama'

Congradulations you have just created a new immortal character.  If you would
like to delete the tempimm account, edit the 'accounts' file in the /TFE/files/
directory and remove the account.  Don't forget to decrement the number at the
top :)


Note on Ports etc.
------------------

The ports that are bound for this release are 4000 for the java entrance and
4269 for the normal entrance.  This is due to caldera open-linux's reluctance
to let you rebind port 23.  Switching what port includes making two changes,
both to the call in tfe.cc and to the define in define.h.  Also note
that the host name is determine by a gethostbyname function call.  If your
hostname is not resolvable, it tries to set the host to forestsedge.com.  To
change this default, edit network.cc.  Do a search down for forestsedge, and
change the line to read what ever you want as host name.  May be a good idea
anyway, but i'll be damned if im setting my hostname as the default :)