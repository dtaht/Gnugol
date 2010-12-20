;; gnugol.el various emacs interface methods for gnugol

;; Copyright (C) 2008 - 2010 Teklibre

;; Author: Michael D. Taht
;; Last update: 
;; Version: 0.3
;; Keywords: search google wikipedia bing
;; URL: http://the-edge.blogspot.com/
;; Contributors:

;; Say is a set of commands to speak the buffer, file, or region
;; via the cepstral/swift speech synthesis engine(s)

;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; This file is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, Inc., 51 Franklin Street, Fifth floor,
;; Boston, MA 02110-1301, USA.

;; Installation

;; (setq gnugol-voice "Amy") ;; Your voice
;; (setq gnugol-level "8") ;; Your desired sound level
;; 

;; (defun gnugol-start-server)

;; (setq gnugol-default-voice nil)

(defconst gnugol-cmd-version "2")

(defgroup say nil
  "Gnugol"
  :prefix "gnugol-"
  :tag    "Gnugol"
  :group  'utilities)

(defcustom gnugol-activated-string t
  "If non-nil, say is activated and version on load"
  :group 'say
  :type 'boolean)

(defcustom gnugol-default-voice nil
  "Choose the voice to use, if nil, use the default. This is very cepstral-specific,
   for example, I use Amy and Diane for announcements and nags, respectively. "
  :group 'say
  :type 'string)

(defcustom gnugol-default-pipe "/dev/shm/SPEECH"
  "We use a named pipe to the daemon. Why? Because we CAN"
  :group 'say
  :type 'string)

(defcustom gnugol-default-cmd "/opt/swift/bin/swift"
  "Sometimes we need to access the speech command directly"
  :group 'say
  :type 'string)

(defcustom gnugol-default-cmd-args-stdin "-f"
  "Sometimes we need to access the speech command directly"
  :group 'say
  :type 'string)

(defcustom gnugol-default-cmd-args-to-file "-o"
  "Sometimes we need to access the speech command directly"
  :group 'say
  :type 'string)


(defun gnugol-version () "Say Version" 
  (interactive)
 (say (concat "The version of say is " gnugol-cmd-version "."))
)

(defun gnugol-with-voice (&optional arg voice)  "Say the argument with a specified voice" 
  (interactive)
 (if (interactive-p) 
     (progn
       (setq arg (read-from-minibuffer "Say: " arg nil nil nil nil t))
       (setq voice (read-from-minibuffer "Voice: " gnugol-default-voice nil nil nil nil t)))
   )
 (if (and (not voice) (gnugol-default-voice)) (setq voice gnugol-default-voice))
 (if voice (setq arg (concat "<voice name=\"" voice "\">" arg "</voice>")))

 (save-excursion
   (with-temp-buffer
     (insert arg)
     (shell-command-on-region nil nil "cat > /dev/shm/SPEECH" nil nil nil nil)
;;     (message "Said %s" arg)
     )))

;; FIXME: I need to figure out how to dump an arg to the *say* buffer so
;; shell escape strings are escaped properly... 
;; And suppress the message

(defun say (&optional arg)  "Say the argument on the speech synthesizer" 
  (interactive)
 (if (interactive-p) 
     (progn
       (setq arg (read-from-minibuffer "Say: " arg nil nil nil nil t))
       ))
 (if gnugol-default-voice (progn 
			 (setq voice gnugol-default-voice)
			 (setq arg (concat "<voice name=\"" voice "\">" arg "</voice>"))))
 (save-excursion
   (with-temp-buffer
     (insert arg)
     (shell-command-on-region nil nil "cat > /dev/shm/SPEECH" nil nil nil nil)
     (message "Said %s" arg)
     )))

;; (defun gnugol-file (&optional arg)
;;   "Say a file"
;;   (shell-command (concat "~/bin/gnugol-file " arg)))

;; Hmm, am I still interactive here?

;; (defun gnugol-buffer2 ()  "Say the contents of the current buffer"
;;   (interactive)
;;   (save-excursion
;;     (insert-string arg) 
    
;;   (say
;;   (shell-command-on-region nil nil  "cat > /dev/shm/SPEECH" nil nil nil nil)
;; )

(defun gnugol-buffer ()  "Say the contents of the current buffer"
  (interactive)
  (save-excursion
  (shell-command-on-region nil nil  "cat > /dev/shm/SPEECH" nil nil nil nil)
  )
)

(defun gnugol-region ()  "Say the region between mark and point"
  (interactive)
  (save-excursion
  (shell-command-on-region (window-point) (mark) "cat > /dev/shm/SPEECH" nil nil nil nil)
  )
)

(defun gnugol-region-to-file (&optional filename mypoint mymark)  
  "Say the region between mark and point"
  (interactive)
  (if (and (interactive-p) (not filename)) 
      (setq filename (nil "Say to File: " nil nil nil nil nil t))
    )
  (if (not mypoint) (setq mypoint (window-point)))
  (if (not mymark) (setq mymark (mark)))
  (shell-command-on-region mypoint mymark "cat > /dev/shm/SPEECH" nil nil nil nil)
  )

(defun gnugol-buffer-to-file (filename) "Say buffer to file"
  (interactive)
  gnugol-region-to-file("/tmp/test.wav" 0 90) ;; fixme - get end of buffer
)

;; Convienent short function

(defun rtb () "Read That Back"  (interactive) (gnugol-region))

;; Fix these

(defun gnugol-at-point () "Say sentence at point" )

(defun gnugol-article () "Say the current article buffer, Skip the headers"
(interactive)
)

(defun gnugol-org () 
(interactive)
"Convert the current org mode buffer into something saner,
 Put emphasis on the headings
 Change Dates to something sane
 Don't say the properties or tags"
(say "Convert the current org mode buffer into something saner,
 Put emphasis on the headings
 Change Dates to something sane
 Don't say the properties or tags"
))

(global-set-key [(Scroll_Lock)] 'rtb)
(global-set-key [(shift Scroll_Lock)] 'gnugol-buffer)

(defun gnugol-version-aloud () "Say the version aloud"
  (interactive)
  (say (concat "Speech synthesis " gnugol-cmd-version " is activated."))
)

;;(if gnugol-activated-string (gnugol-version-aloud))

(provide 'say)

;;2. Changing Voices

 ;;     "This is the default voice. <voice name="David">This is David.</voice> This is the default again. <voice name="Callie">Callie here.</voice>" ;; 
;; 6. Adding Emphasis to Speech

;;      "This is <emphasis level='strong'>stronger</emphasis> than the rest."
;;      "This is <emphasis level='moderate'>stronger</emphasis> than the rest."
;;      "This is <emphasis level='none'>the same as</emphasis> than the rest." 

;; 7. Inserting Recorded Audio Files

;;      "Please leave your message after the tone <audio src='beep.wav' />"
;;      "<audio src='non_existing_file.au'>File could not be played.</audio>" 
