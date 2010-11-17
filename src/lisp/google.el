;;; google.el --- Use current XEmacs selection as Google query.
;; Copyright (C) 2001 Erik Arneson
;;
;; Author:     Erik Arneson 
;; Maintainer: erik@aarg.net
;; Created:    Jun-06-2001
;; Version:    0.50
;; Keywords:   extensions, web, search, google
;;
;; This file is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 2, or (at your
;; option) any later version.
;;
;; This file is distributed in the hope that it will be useful, but
;; WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;; General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this file; see the file COPYING.  If not, write to the
;; Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
;; 02111-1307, USA.
;;
;;; Commentary:
;;
;; Map this to a global key.  I use `C-c s'.  Highlight something in
;; XEmacs, hit this key sequence, and the highlighted region will be
;; sent to Google.

;;; Changelog (sort of)
;;
;; I hacked this a bit, created google-it and made it interactive.
;; Removed XEmacs-isms - buf was bound to (current-buffer) and passed
;; to region-{beginning,end} and buffer-substring, but those use
;; the current buffer by default (and in Emacs, they can't use others).
;; Also conditionalised use of region-exists-p.
;; --Magnus Henoch

(require 'url)

(defvar google-search-maxlen 50
  "Maximum string length of search term.  This prevents you from accidentally
sending a five megabyte query string to Netscape.")

(defun google-it (search-string)
  "Search for SEARCH-STRING on google."
  (interactive "sSearch for: ")
  (browse-url (concat "http://www.google.com/search?client=emacs&q="
                      (url-hexify-string
                       (encode-coding-string search-string 'utf-8)))))

(defun google-search-selection ()
  "Create a Google search URL and send it to your web browser."
  (interactive)
  (let (start end term url)
    (if (or (not (fboundp 'region-exists-p)) (region-exists-p))
        (progn
          (setq start (region-beginning)
                end   (region-end))
          (if (> (- start end) google-search-maxlen)
              (setq term (buffer-substring start (+ start google-search-maxlen)))
            (setq term (buffer-substring start end)))
          (google-it term))
      (beep)
      (message "Region not active"))))

(provide 'google-search)
;;; End of file.

