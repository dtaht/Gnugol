;; gnugol.el - Web search using the gnugol command line utility
;; Copyright (C) 2010 Dave Täht
;; License:    GNU Public License, version 3
;; Author:     Dave Taht
;; Maintainer: d AT @ taht.net
;; Created:    Dec-2008
;; Version:    0.03
;; Keywords:   extensions, web, search, google

;; This is an interface to the gnugol command line
;; web search utility, which can be obtained at:
;; http://gnugol.taht.net

;; I find gnugol useful enough to stick on a function key
;; in my keybindings.el file elsewhere.
;; (define-key global-map [f6] 'gnugol)

;; FIXME: Convert all to defcustom and add support for args

(defcustom gnugol-cmd "gnugol -o org"
  "Shell command to invoke gnugol."
  :type 'string
  :group 'gnugol)

(defcustom gnugol-default-engine nil
  "Default search engine backend for gnugol."
  :type 'string
  :group 'gnugol)

(defcustom gnugol-default-nresults 4
  "Default number of results for gnugol to return."
  :type 'integer
  :group 'gnugol)

(defcustom gnugol-default-base-output-format "org"
  "Default output format for gnugol."
  :type 'string
  :group 'gnugol)

(defcustom gnugol-default-output-buffer "*gnugol*"
  "Output buffer. Set this to ~/org/gnugol.org if you want to keep your history."
  :type 'string
  :group 'gnugol)

(defcustom gnugol-search-maxlen 200
  "Maximum string length of search term. This saves on sending accidentally large queries to the search engine."
  :type 'integer
  :group 'gnugol)

(defcustom gnugol-default-search-maxwords 4
  "Maximum number of words to search for."
  :type 'integer
  :group 'gnugol)

(defcustom gnugol-default-output-mode-sensitive nil
  "Be sensitive to the current buffer mode. Output results in that format."
  :type 'boolean
  :group 'gnugol)

(defcustom gnugol-default-timestamp-output nil
  "Timestamp the output."
  :type 'boolean
  :group 'gnugol)

;; FIXME Haven't decided if doing a ring buffer would be useful

(defcustom gnugol-ring-max 16
  "Maximum length of search ring before oldest elements are thrown away."
  :type 'integer
  :group 'gnugol)

;; FIXME figure out how to search the buffer-modes

(defun gnugol-get-output-mode
  "Get the gnugol output mode from the current buffer mode."
  (if gnugol-default-output-mode-sensitive 
    (	) 
    ("org")))

;; FIXME: gnugol should act more like "woman"

;; FIXME: If there is a visible gnugol buffer change focus to that rather than the current
;; FIXME: Add hooks for further washing the data

;; FIXME: add next, prev, and refresh buttons 
;        [[gnugol: :next :pos 4 str][more]] [[gnugol: prev][prev]] [[refresh]]
;; FIXME: Document this shell shortcut into org
;; FIXME: search forward in the buffer for an existing
;;        set of keywords rather than call gnugol
;;        (if (search-forward (concat "[[Search: " str )) () (gnugol-cmd str)))?
;; FIXME: Sanitize the shell arguments to prevent abuse !! For example no CRs
;;        regexp? Maybe the shell does it for me? What?
;; FIXME: actually, going to the 4th char in on the title would work best

(defun gnugol (str)
  "search the web via gnugol, bring up results in org buffer"
  (interactive "sSearch: ")
  (if (< (length str) gnugol-search-maxlen)
      (let (newbuffer)
	(setq newbuffer (get-buffer-create gnugol-default-output-buffer))
	(set-buffer newbuffer)
	(org-mode)
	(goto-char (point-min))
	;; FIXME what we want to do is something like this but I'm getting it wrong
	;; (if (search-forward (concat "[Search: " str "]")) () 
	(save-excursion 
	  (insert-string (concat "* [[gnugol: " str "][Search: " str "]]"))
	  (insert (shell-command-to-string (concat gnugol-cmd " " str )))
	  (switch-to-buffer newbuffer)
	  ))
    ( (beep) (message "search string too long")))) 

(defun gnugol-search-selection ()
  "Do a gnugol search based on a region"
  (interactive)
  (let (start end term url)
    (if (or (not (fboundp 'region-exists-p)) (region-exists-p))
        (progn
          (setq start (region-beginning)
                end   (region-end))
          (if (> (- start end) gnugol-search-maxlen)
              (setq term (buffer-substring start (+ start gnugol-search-maxlen)))
            (setq term (buffer-substring start end)))
          (gnugol term))
      (beep)
      (message "Region not active"))))


;; FIXME: NOTHING BELOW HERE ACTUALLY WORKS YET
;; FIXME: For the into-pt stuff, be sensitive to the mode
;;        If I'm in markdown format, return markdown
;;        org, do org
;;        html, do html. Etc.
;;        C mode, put it in comments
;;        etc
;; FIXME: simplify navigation in org-mode buffer with minor mode
;;        add n and p to move to the links? CNTRL-arrows are good enough 
;;        
;; FIXME: Add robust interface
;; gnugol-thing-at-pt
;; gnugol-into-pt
;; gnugol-thing-at-pt-into-pt

;; add hooks for additional modes - 

(provide 'gnugol)