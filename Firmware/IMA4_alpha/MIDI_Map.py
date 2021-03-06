#     Interfaz MIDI Argentina 4
# www.github.com/luxarts/IMA-4
# Created by LuxARTS, OG-Jonii & pablonobile99
#               2016
# Note and CC Mappings for the IMA4_alpha script (APC emulation) are defined in this file
# Values may be edited with any text editor (but avoid using tabs for indentation)

# Combination Mode offsets
# ------------------------

TRACK_OFFSET = -1 #offset from the left of linked session origin; set to -1 for auto-joining of multiple instances
SCENE_OFFSET = 0 #offset from the top of linked session origin (no auto-join)

# Buttons / Pads
# -------------
# Valid Note/CC assignments are 0 to 127, or -1 for NONE
# Duplicate assignments are permitted

BUTTONCHANNEL = 0 #Channel assignment for all mapped buttons/pads; valid range is 0 to 15 ; 0=1, 1=2 etc.
MESSAGETYPE = 0 #Message type for buttons/pads; set to 0 for MIDI Notes, 1 for CCs.
        #When using CCs for buttons/pads, set BUTTONCHANNEL and SLIDERCHANNEL to different values.

# General
PLAY = 97 #Global play
STOP = 98 #Global stop
REC = 99 #Global record
TAPTEMPO = -1 #Tap tempo
NUDGEUP = -1 #Tempo Nudge Up
NUDGEDOWN = -1 #Tempo Nudge Down
UNDO = -1 #Undo
REDO = -1 #Redo
LOOP = -1 #Loop on/off
PUNCHIN = -1 #Punch in
PUNCHOUT = -1 #Punch out
OVERDUB = -1 #Overdub on/off
METRONOME = -1 #Metronome on/off
RECQUANT = -1 #Record quantization on/off
DETAILVIEW = -1 #Detail view switch
CLIPTRACKVIEW = -1 #Clip/Track view switch

# Device Control
DEVICELOCK = -1 #Device Lock (lock "blue hand")
DEVICEONOFF = -1 #Device on/off
DEVICENAVLEFT = -1 #Device nav left
DEVICENAVRIGHT = -1 #Device nav right
DEVICEBANKNAVLEFT = -1 #Device bank nav left
DEVICEBANKNAVRIGHT = -1 #Device bank nav right
DEVICEBANK = (-1, #Bank 1 #All 8 banks must be assigned to positive values in order for bank selection to work
              -1, #Bank 2
              -1, #Bank 3
              -1, #Bank 4
              -1, #Bank 5
              -1, #Bank 6
              -1, #Bank 7
              -1, #Bank 8
              )

# Arrangement View Controls
SEEKFWD = -1 #Seek forward
SEEKRWD = -1 #Seek rewind

# Session Navigation (aka "red box")
SESSIONUP = 112 #Session up
SESSIONDOWN = 113 #Session down
SESSIONLEFT = 114 #Session left
SESSIONRIGHT = 115 #Session right
ZOOMUP = 116 #Session Zoom up (Moves 4 tracks/scenes)
ZOOMDOWN = 117 #Session Zoom down
ZOOMLEFT = 118 #Session Zoom left
ZOOMRIGHT = 119 #Session Zoom right

# Track Navigation
TRACKLEFT = -1 #Track left
TRACKRIGHT = -1 #Track right

# Scene Navigation
SCENEUP = -1 #Scene down
SCENEDN = -1 #Scene up

# Scene Launch
SELSCENELAUNCH = -1 #Selected scene launch
SCENELAUNCH = (120, #Scene 1 Launch
               121, #Scene 2
               122, #Scene 3
               123, #Scene 4
               -1, #Scene 5
               )

# Clip Launch / Stop
SELCLIPLAUNCH = -1 #Selected clip launch
STOPALLCLIPS = -1 #Stop all clips

# 4x4 Matrix note assignments
# Track no.:     1   2   3   4
CLIPNOTEMAP = ((0,   1,  2,  3), #Row 1
               (16, 17, 18, 19), #Row 2
               (32, 33, 34, 35), #Row 3
               (48, 49, 50, 51), #Row 4
               )

# Track Control
MASTERSEL = -1 #Master track select
TRACKSTOP = (124, #Track 1 Clip Stop
             125, #Track 2
             126, #Track 3
             127, #Track 4
             -1, #Track 5
             -1, #Track 6
             -1, #Track 7
             -1, #Track 8
             )
TRACKSEL = (-1, #Track 1 Select
            -1, #Track 2
            -1, #Track 3
            -1, #Track 4
            -1, #Track 5
            -1, #Track 6
            -1, #Track 7
            -1, #Track 8
            )
TRACKMUTE = (100, #Track 1 On/Off
             101, #Track 2
             102, #Track 3
             103, #Track 4
             -1, #Track 5
             -1, #Track 6
             -1, #Track 7
             -1, #Track 8
             )
TRACKSOLO = (104, #Track 1 Solo
             105, #Track 2
             106, #Track 3
             107, #Track 4
             -1, #Track 5
             -1, #Track 6
             -1, #Track 7
             -1, #Track 8
             )
TRACKREC = (108, #Track 1 Record
            109, #Track 2
            110, #Track 3
            111, #Track 4
            -1, #Track 5
            -1, #Track 6
            -1, #Track 7
            -1, #Track 8
            )


# Pad Translations for Drum Rack

PADCHANNEL = 1 # MIDI channel for Drum Rack notes
DRUM_PADS = (0, 1, 2, 3, # MIDI note numbers for 4 x 4 Drum Rack
             4, 5, 6, 7, # Mapping will be disabled if any notes are set to -1
             8, 9, 10, 11, # Notes will be "swallowed" if already mapped elsewhere
             12, 13, 14, 15,
             )

# Sliders / Knobs
# ---------------
# Valid CC assignments are 0 to 127, or -1 for NONE
# Duplicate assignments will be ignored

SLIDERCHANNEL = 0 #Channel assignment for all mapped CCs; valid range is 0 to 15
TEMPO_TOP = 180.0 # Upper limit of tempo control in BPM (max is 999)
TEMPO_BOTTOM = 80.0 # Lower limit of tempo control in BPM (min is 0)

TEMPOCONTROL = -1 #Tempo control CC assignment; control range is set above
MASTERVOLUME = 12 #Master track volume
CUELEVEL = -1 #Cue level control
CROSSFADER = -1 #Crossfader control

TRACKVOL = (0, #Track 1 Volume
            1, #Track 2
            2, #Track 3
            3, #Track 4
            -1, #Track 5
            -1, #Track 6
            -1, #Track 7
            -1, #Track 8
            )
TRACKPAN = (-1, #Track 1 Pan
            -1, #Track 2
            -1, #Track 3
            -1, #Track 4
            -1, #Track 5
            -1, #Track 6
            -1, #Track 7
            -1, #Track 8
            )
TRACKSENDA = (-1, #Track 1 Send A
              -1, #Track 2
              -1, #Track 3
              -1, #Track 4
              -1, #Track 5
              -1, #Track 6
              -1, #Track 7
              -1, #Track 8
              )
TRACKSENDB = (-1, #Track 1 Send B
              -1, #Track 2
              -1, #Track 3
              -1, #Track 4
              -1, #Track 5
              -1, #Track 6
              -1, #Track 7
              -1, #Track 8
              )
TRACKSENDC = (-1, #Track 1 Send C
              -1, #Track 2
              -1, #Track 3
              -1, #Track 4
              -1, #Track 5
              -1, #Track 6
              -1, #Track 7
              -1, #Track 8
              )
PARAMCONTROL = (4, #Param 1 #All 8 params must be assigned to positive values in order for param control to work
                5, #Param 2
                6, #Param 3
                7, #Param 4
                8, #Param 5
                9, #Param 6
                10, #Param 7
                11, #Param 8
                )
