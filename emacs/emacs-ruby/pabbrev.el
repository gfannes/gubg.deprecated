;;; Saved through ges-version 0.3.3dev at 2003-12-06 13:09
;;; ;;; From: Phillip Lord <p.lord@russet.org.uk>
;;; ;;; Subject: pabbrev
;;; ;;; Newsgroups: gnu.emacs.sources
;;; ;;; Date: 25 Nov 2003 10:58:06 +0000
;;; ;;; Organization: Dept of Computer Science, University of Manchester, U.K.

;;; [1. text/plain]




;;; Next version of my predictive abbreviation expansion, which maybe
;;; fixes some problems with the first version. 

;;; Maybe not. post-command-hook are nasty to debug....

;;; Please let me know. 

;;; Phil



;;; [2. application/emacs-lisp; pabbrev.el]

;;; pabbrev.el --- Predictive abbreviation expansion

;; $Revision: 1.1 $
;; $Date: 2005/10/17 20:18:50 $

;; This file is not part of Emacs

;; Author: Phillip Lord <p.lord@russet.org.uk>
;; Maintainer: Phillip Lord <p.lord@russet.org.uk>
;; Website: http://www.russet.org.uk

;; COPYRIGHT NOTICE
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;; Commentary:
;;
;; THIS IS AN EARLY RELEASE.  It currently works for me, but may not
;; work for you.  In most cases the worst that is likely to happen is
;; that you will end up with some previously offered expansions
;; hanging in the buffer.  You can just delete these in the normal
;; way.
;;
;; THIS IS A REASONABLE EARLY RELEASE.  Having said that this is an
;; early release, and there are some possibilities of it crashing, it
;; should crash safely, so you won't loose anything, and I am using it
;; routinely.  So give it a try if you think it will work for you.
;;
;; The code provides a abbreviation expansion for Emacs.  Its fairly
;; similar to "dabbrev" expansion, which works based on the contents
;; of the current buffer (or other buffers).
;;
;; Predictive abbreviation expansion works based on the previously
;; written text.  Unlike dynamic abbreviation, the text is analysed
;; during idle time, while Emacs is doing nothing else.  The advantage
;; of this is that its very quick to look up potential abbreviations,
;; which means that the can be constantly displayed, without
;; interfering with the user as they type. Certainly it works for me,
;; on an old laptop, typing as fast as I can (which is fast, since I
;; learnt to type with four fingers). 
;;
;; pabbrev's main entry point is through the minor mode
;; `pabbrev-mode'.  There is also a global minor mode, called
;; `global-pabbrev-mode', which does the same in all appropriate
;; buffers.
;;
;; The current user interface looks like so...
;; 
;; p[oint]
;; pr[ogn]
;; pre[-command-hook]
;; pred[ictive]
;;
;; As the user types the sytem narrows down the possibilities.  The
;; narrowing is based on how many times the words have been used
;; previously.  By hitting [tab] at any point the user can complete the
;; word.  The [tab] key is normally bound to `indent-line'.
;; `pabbrev-mode' preserves access to this command (or whatever else
;; [tab] was bound to), if there is not current expansion.
;;
;; But is this actually of any use? Well having use the system for a
;; while now, I can say that it is sometimes.  I originally thought
;; that it would be good for text, but in general its not so
;; useful.  By the time you have realeased that you have an expansion
;; that you can use, hit tab, and checked that its done the right
;; thing, you could have just typed the word directly in.  It's much
;; nicer in code containing buffers, where there tend to be lots of
;; long words, which is obviously where an abbreviation expansion
;; mechanism is most useful.
;;
;; Currently pabbrev builds up a dictionary on a per major-mode basis.
;; While pabbrev builds up this dictionary automatically, you can also
;; explicitly add a buffer, or a region to the dictionary with
;; `pabbrev-scavenge-buffer', or `pabbrev-scavenge-region'.  There is
;; also a command `pabbrev-scavenge-some' which adds some words from
;; around point.  pabbrev remembers the word that it has seen already,
;; so run these commands as many times as you wish. 
;;
;; Although the main data structures are efficient during typing, the
;; pay off cost is that they can take a reasonable amount of time, and
;; processor power to gather up the words from the buffer. There are
;; two main settings of interest to reduce this, which are
;; `pabbrev-scavenge-some-chunk-size' and
;; `pabbrev-scavenge-on-large-move'.  `pabbrev-mode' gathers text from
;; around point when point has moved a long.  This means symbols
;; within the current context should be in the dictionary, but it can
;; make Emacs choppy, in handling.  Either reduce
;; `pabbrev-scavenge-some-chunk-size' to a smaller value, or
;; `pabbrev-scavenge-on-large-move' to nil to reduce the effects of
;; this.
;;
;; NOTE: There are a set of standard conventions for Emacs minor
;; modes, particularly with respect to standard key bindings, which
;; pabbrev somewhat abuses.  The justification for this is that the
;; whole point of pabbrev mode is to speed up typing.  Access to its
;; main function has to be on a very easy to use keybinding.  The tab
;; seems to be a good choice for this.  By preserving access to the
;; original tab binding when there is no expansion, pabbrev mostly
;; "does what I mean", at least in my hands.
;; 

;;; Installation:
;;
;; To install this file place in your `load-path', and add
;; 
;; (require 'pabbrev)
;;
;; to your .emacs

;;; Status:
;;
;; At the moment this seems to be working mostly, although
;; occasionally it seems to leave an expansion in the buffer.
;; I wrote this on an Emacs 21.0 prerelease, that I haven't upgraded
;; yet. It has not been tested on XEmacs, but I'd be interested to
;; know if it works.

;;; Limitations:
;;
;; pabbrev is not finished yet, so there are quite a few. There are
;; four main ones, two with the UI, and two with the core data
;; structures. These are:-
;;
;; 1) I would like some abbreviation expansion cycling method, or a
;; system where you can view lots of possibilities at once. I don't
;; cycling enough, like `dabbrev-expand' is enough, because there is
;; no way for the user to a priori judge what the expansion cycling
;; order is going to be. I've started work on this already. You can
;; see the results with `pabbrev-suggestions-toggle'. The core data
;; structures are fast enough for it to work. I haven't worked out a
;; good way of actually selecting the words yet, without taking over a
;; lot of keypress space. Cycling is one possibility, taking over the
;; function keys is another.
;;
;; 2) A minimum matching substring would be nice, probably from the,
;; say, top ten best matches. I'm not sure how this would be
;; keybound. One thought is to do othis with Ctrl-Tab. The display
;; would either just use font lock, or multiple square brackets. So
;; the expansion would look
;; pa[bbrev][-mode]
;; where pabbrev is the minimum matching substring for the top ten
;; suggestions, and pabbrev-mode is the most commonly occuring match.
;; As an alternative it might show pa[bbrev-mode], with "abbrev" in
;; one font-lock-face, and "-mode" in a different.
;;
;; 3) There is no method for removing usages from the
;; dictionaries. They just get bigger. This hasn't been a problem so
;; far, but with long running Emacs it might be.
;;
;; 4) I think it would be nice to save the dictionaries, or offer
;; facilities for doing so, before Emacs is killed. This would clearly
;; depend on point 3 also. I'm not sure whether this is possible in a
;; reasonable length of time. `pabbrev-debug-print-hashes' is
;; certainly pretty slow.
;;
;; 5) I think that the scavenge functions are more computationally
;; intensive than they need to be. They generally run in the idle
;; cycle so its not a disaster. However more efficiency would mean the
;; buffer could be gathered more quickly. 
;;
;; On a more general note, the code base could do with a bit of
;; re-ordering, and it needs custom support.

;;; Implementation notes:
;;
;; The core data structures are two hashes. The first of which looks
;; like this...
;; "the" -> ("the" . 5)
;; "there" -> ("there" . 3)
;; 
;; I call this the usage hash, as it stores the total number of times
;; each word has been seen.
;;
;; The second hash which is called the prefix hash. It stores
;; prefixes, and usages...
;;
;; "t"->
;; (("the" . 64)
;;  ("to" . 28)
;;  ("t" . 22)
;;  ("this" . 17))
;;
;; "th"->
;; (("the" . 64)
;;  ("this" . 17)
;;  ("that" . 7))
;;
;; "the"->
;; (("the" . 64)
;;  ("there" . 6)
;;  ("then" . 3)
;;  ("these" . 1))
;;
;; The alist cons cells in the first hash are conserved in the second,
;; but the alists are not. The alist in the second hash is always
;; sorted, on the basis of word usage.
;;
;; The point with this data structure is that I can find word usage
;; in constant time, from the first hash, and completions for a given
;; prefix, also in constant time. As access to completions happens as
;; the user types speed is more important here, than at during
;; update, which is why the prefix hash maintains sorted alists. This
;; is probably at the cost of slower updating of words.

;;; Acknowledgements;
;;
;; Once again I need to thank Stefan Monnier, for his comments on my
;; code base. Once day I will write a minor mode which Stefan Monnier
;; does not offer me advice on, but it would appear that this day has not
;; yet arrived!
;;
;; I should also thank Kim F. Storm (and in turn Stephen Eglen), as
;; the user interface for this mode has been heavily influenced by
;; ido.el, a wonderful package which I use every day.

;;; History:
;;
;; $Log: pabbrev.el,v $
;; Revision 1.1  2005/10/17 20:18:50  cvsuser
;; ruby electric and pabbrev added
;;
;; Revision 1.5  2003/11/24 11:08:46  phillord
;; Extended documentation.
;; Improved previously bound key handling.
;; Added suggestions code, for displaying additional matches.
;;
;; Revision 1.2  2003/11/17 14:34:35  phillord
;; Bug in code to catch bug with global mode.
;; Prevent mark-words from affecting modification status.
;;
;; Revision 1.1  2003/11/17 14:22:44  phillord
;; Initial checkin
;;
;;

;;; Code:
(eval-when-compile (require 'cl))
(require 'thingatpt)

;;; Start data structures
(defvar pabbrev-usage-hash-modes nil
  "List of modes with associated usage dictionaries.")

(defvar pabbrev-prefix-hash-modes nil
  "List of modes with associated prefix dictionaries.")

(defun pabbrev-get-usage-hash()
  "Returns the usage hash for this buffer."
  (let((hash (get major-mode 'pabbrev-usage-hash)))
    (unless hash
      (put major-mode 'pabbrev-usage-hash
           (setq hash
                 (make-hash-table :test 'equal)))
      (push major-mode pabbrev-usage-hash-modes))
    hash))

(defun pabbrev-get-usage-dictionary-size()
  "Returns the size of the usage hash."
  (hash-table-count (pabbrev-get-usage-hash)))

(defun pabbrev-get-total-usages-dictionary()
  "Returns the total number of usages from the usage hash"
  (interactive)
  (let ((size 0))
    (maphash
     (lambda(key value)
       (setq size (+ size (cdr value))))
     (pabbrev-get-usage-hash))
    size))

(defun pabbrev-get-prefix-hash()
  "Returns the prefix hash for the current buffer."
  (let((hash (get major-mode 'pabbrev-prefix-hash)))
    (unless hash
      (put major-mode 'pabbrev-prefix-hash
           (setq hash
                 (make-hash-table :test 'equal)))
      (push major-mode pabbrev-prefix-hash-modes))
    hash))

(defun pabbrev-add-word-usage (word)
  "Add a WORD to the usage hash.
This is a function internal to the data structures.  The
`pabbrev-add-word' is the main entry point to this functionality."
  (let ((value
         (gethash
          ;; look for word usage cons we need a cons, but the last
          ;; value is irrelevant.
          word
          (pabbrev-get-usage-hash))))
    ;; so now we have cons, or nil
    (if value
        ;; increment occurences
        (setcdr
         value (+ 1 (cdr value)))
      ;; we have no so make is
      (setq value
            (cons word 1)))
    ;; so now we the cons cell for sure
    ;; possible we should do this above, as I think it only needs
    ;; doing for a new cons.
    (puthash word value (pabbrev-get-usage-hash))
    value))

  
(defun pabbrev-add-word-cons-with-prefix (prefix conscell)
  "Add a word usage, and a PREFIX.
This function is internal to the data structures, and should normally
only be called, by `pabbrev-add-word'.  CONSCELL should be cons
returned from `pabbrev-add-word-usage', while PREFIX should be a
prefix of the from the cons cell."
  (let
      ;; this should be an alist or nil
      ((value (gethash prefix
                       (pabbrev-get-prefix-hash))))
    (if value
        ;; so we have an alist. Has our word been added to this alist
        ;; before? If not, do so. If it has been added, then it will
        ;; have been updated with the addition of the word
        (if (not
             (member conscell value))
            (setq value (cons conscell value)))
      ;; nothing in there, so create an alist with
      ;; a single element
      (setq value (list conscell)))
    ;; so we now have the value alist...sort it
    (puthash prefix
             ;; this sort is bit poor. It should be possible to do
             ;; this in less than linear time, rather than n(log-n) as
             ;; now. I think most of the time is spent entering the lambda
             ;; function. Possibly the sort could be done on removing
             ;; the value from the hash. The irony is that the sort is
             ;; more or less sorted from the start, so a bubble sort
             ;; would work in linear time.
             (sort value
                   ;'pabbrev-comparitor-function)
                   (lambda(a b)
                     (> (cdr a) (cdr b))))
             (pabbrev-get-prefix-hash))))
  
(defun pabbrev-add-word (word)
  "Add the usage of a WORD to the current dictionary."
  (let ((conscell
         (pabbrev-add-word-usage word)))
    (dotimes (i (- (length word) 1))
      (pabbrev-add-word-cons-with-prefix
       (substring word 0 (1+ i))
       conscell))))

(defun pabbrev-fetch-all-suggestions-for-prefix(prefix)
  "Returns the suggestions for a given PREFIX.
Results are an alist, with cons with car of the word, and cdr of the
number of usages seen so far. This alist should NOT be altered, its
it's ordering is part of the core data structures"
  (gethash prefix (pabbrev-get-prefix-hash)))


;; Which completes the core data structures.



;; This code provides the minor mode which displays, and accepts
;; abbreviations.


(defvar pabbrev-mode-map (make-keymap)
  "Keymap for pabbrev-minor-mode.")
(define-key pabbrev-mode-map "\t" 'pabbrev-expand-maybe)

(easy-mmode-define-minor-mode pabbrev-mode
                              "Toggle pabbrev mode"
                              nil
                              " pabbrev"
                              pabbrev-mode-map
                              (when (and pabbrev-mode
                                         buffer-read-only)
                                (error "Can not use pabbrev-mode in read only buffer")))

(easy-mmode-define-global-mode global-pabbrev-mode
                               pabbrev-mode pabbrev-global-mode)

(defvar pabbrev-global-mode-excluded-modes '(shell-mode)
  "Will not activate function `global-pabbrev-mode' in buffers with a major mode in this list.")

(defvar pabbrev-global-mode-not-buffer-names '("*Messages*")
  "Will not activate function `global-pabbrev-mode' if buffers have this name." )

(defun pabbrev-global-mode()
  "Switch on `pabbrev-mode' in current buffer if appropriate.
Currently appropriate means, if the buffer is not read only, and is
not a minibuffer."
  (unless (or buffer-read-only
              pabbrev-mode
              (member major-mode pabbrev-global-mode-excluded-modes)
              ;; don't turn on in non listable buffers
              (equal (substring (buffer-name) 0 1) " ")
              (member (buffer-name) pabbrev-global-mode-not-buffer-names)
              (window-minibuffer-p (selected-window)))
    (let
        ;; set the chunk size low, or the global mode takes for ever
        ;; to switch on
        ((pabbrev-scavenge-some-chunk-size 0))
      (pabbrev-mode))))

;; hooks for switching on and off.
(add-hook 'pabbrev-mode-on-hook
          'pabbrev-mode-on)
(add-hook 'pabbrev-mode-off-hook
          'pabbrev-mode-off)

(defvar pabbrev-marker nil
  "Location of current insertion, or nil.
This variable is not actually a marker, but a cons of
start and end positions")
(make-variable-buffer-local 'pabbrev-marker)

(defvar pabbrev-expansion nil
  "Currently displayed expansion, or nil.")
(make-variable-buffer-local 'pabbrev-expansion)

(defvar pabbrev-marker-last-expansion nil
  "Marks where the last possible expansion was.")
(make-variable-buffer-local 'pabbrev-marker-last-expansion)

(defvar pabbrev-marker-distance-before-scavenge 2000
  "Minimal distance moved before we wish to scavenge.")

(defun pabbrev-mode-on()
  "Turn `pabbrev-mode' on."
  (add-hook 'pre-command-hook 'pabbrev-pre-command-hook nil t)
  (add-hook 'post-command-hook 'pabbrev-post-command-hook nil t))

(defun pabbrev-mode-off()
  "Turn `pabbrev-mode' off."
  ;; we have to remove the binding for tab. Other wise next time we
  ;; switch the mode on, this binding will be found, and set for
  ;; pabbrev-tab-previously-defined
  (remove-hook 'pre-command-hook 'pabbrev-pre-command-hook t)
  (remove-hook 'post-command-hook 'pabbrev-post-command-hook t))

(defvar pabbrev-expand-after-command-list
  '(self-insert-command mouse-set-point delete-char backward-delete-char-untabify)
  "Set of commands after which expansion should be offered.")

;;(defun test()(interactive)(pabbrev-post-command-hook))
(defun pabbrev-post-command-hook()
  "Offer expansion if appropriate.
This function is normally run off the `post-command-hook'."
  (condition-case err
      (save-excursion
        (let ((word (pabbrev-thing-at-point))
              (bounds (pabbrev-bounds-of-thing-at-point))
              (suggestions))
          (if (and
               ;; we have just had an appropriate command
               (memq last-command pabbrev-expand-after-command-list)
               ;; is word at point
               word
               ;; we are at the end of it.
               (= (point) (cdr bounds))
               ;; and we have some suggestions.
               (setq suggestions (pabbrev-fetch-all-suggestions-for-prefix word)))
              (progn
                (pabbrev-insert-suggestion word (cdr bounds) suggestions)
                (pabbrev-post-command-check-movement)
                (pabbrev-post-command-show-suggestions suggestions))
            (pabbrev-post-command-delete-suggestions))))
    (error
     (pabbrev-command-hook-fail err "post" ))))

(defun pabbrev-pre-command-hook()
  "Remove offering expansion from the buffer, if present.
This function is normally run off the `pre-command-hook'"
  (condition-case err
      (progn
        (unless (memq this-command
                      pabbrev-expand-commands)
          (setq pabbrev-expansion nil))
        (if pabbrev-marker
            (pabbrev-save-buffer-modified-p
             (pabbrev-delete-overlay)
             (delete-region (car pabbrev-marker) (cdr pabbrev-marker))
             (setq pabbrev-marker nil))))
    ;;catch the error
    (error
     (pabbrev-command-hook-fail err "pre"))))

(defun pabbrev-command-hook-fail(err hook)
  "Advise user of a failure command-hooks.
This function should only run as the result of a bug.
A message is sent, as we can do little else safely,
on the `post-command-hook', or `pre-command-hook'."
  (message "pabbrev mode has failed on %s hook: %s "
           hook (error-message-string err))
  (signal err nil))

(defmacro pabbrev-save-buffer-modified-p(&rest body)
  "Eval BODY without affected buffer modification status"
  `(let ((buffer-modified (buffer-modified-p)))
     ,@body
     (set-buffer-modified-p buffer-modified)))


(defun pabbrev-marker-last-expansion()
  "Fetch marker for last offered expansion."
  (unless
      pabbrev-marker-last-expansion
    (setq pabbrev-marker-last-expansion
          (set-marker (make-marker)
                      (point) (current-buffer))))
  pabbrev-marker-last-expansion)

(defun pabbrev-update-marker()
  (set-marker (pabbrev-marker-last-expansion)
              (point) (current-buffer)))

;;(setq pabbrev-scavenge-on-large-move nil)
(defvar pabbrev-scavenge-on-large-move t
  "If non NIL, scavenge when a large buffer move has occured.
This can make Emacs' handling a little bumpy.  See also
`pabbrev-scavenge-some-chunk-size', as reducing this, or increasing
`pabbrev-marker-distance-before-scavenge'  is an alternative
to setting this to nil")

(defun pabbrev-post-command-check-movement()
  (let ((distance
         (abs (- (point) (marker-position
                          (pabbrev-marker-last-expansion))))))
    (if (> distance pabbrev-marker-distance-before-scavenge)
        ;; we have moved a lot in the buffer
        (progn
          (pabbrev-debug-message "Scavenge due to buffer marker")
          (pabbrev-scavenge-some)
          (pabbrev-update-marker)))))

(defvar pabbrev-overlay nil
  "Overlay for offered completion.")
(make-variable-buffer-local 'pabbrev-overlay)

(defun pabbrev-set-overlay(start end)
  "Move overlay to START END location."
  (unless pabbrev-overlay
    (setq pabbrev-overlay (make-overlay 0 0)))
  (overlay-put pabbrev-overlay
               'face 'font-lock-type-face)
  (move-overlay pabbrev-overlay start end (current-buffer)))

(defun pabbrev-delete-overlay()
  "Make overlay invisible."
  (if pabbrev-overlay
      (delete-overlay pabbrev-overlay)))

(defun pabbrev-insert-suggestion(prefix end suggestions)
  "Insert a suggestion into the buffer.
The suggestion should start with PREFIX, and be entered
at buffer position END."
  (let* ((suggestion
          (car suggestions)))
    (let ((expansion
           (if suggestion
               (substring (car suggestion)
                          (length prefix))
             "")))
      (save-excursion
        (pabbrev-save-buffer-modified-p
         (insert
          "[" expansion "]" )
         (let ((point-1 (- (point) 1)))
           (pabbrev-set-overlay
            (- point-1 (length expansion)) point-1)))
        (setq
         pabbrev-expansion expansion
         pabbrev-marker
         (cons end (point)))))))

(defun pabbrev-expand-maybe()
  "Expand abbreviation, or run previous command.
If there is no expansion the command on `pabbrev-tab-previously-bound'
will be run instead."
  (interactive)
  ;; call expand if we can
  (if pabbrev-expansion
      (pabbrev-expand)
    ;; hopefully this code will actually work as intended now. It's
    ;; been around the house a few times already!
    (let ((prev-binding
           (let ((pabbrev-mode nil))
             (key-binding (char-to-string last-command-event)))))
      (if (and (fboundp prev-binding)
               (not (eq prev-binding 'pabbrev-expand-maybe)))
          (funcall prev-binding)))))


;;           ;; I think that I have this worked out now.
;;           (if (eq prev-binding 'pabbrev-expand-maybe)
;;               (message "pabbrev known bug! Avoiding recursive tab")
;;             (funcall prev-binding))))))

;;     (define-key pabbrev-mode-map "\t" nil)
;;     (let ((tunneled-keybinding (key-binding "\t")))
;;       (if (and (fboundp tunneled-keybinding)
;;                (not (eq tunneled-keybinding 'pabbrev-expand-maybe)))
;;           (funcall tunneled-keybinding)))
;;     (define-key pabbrev-mode-map "\t" 'pabbrev-expand-maybe)))


(defun pabbrev-expand()
  "Expand abbreviation"
  (interactive)
  (if pabbrev-expansion
      (insert pabbrev-expansion)
    (message "No expansion"))
  (setq pabbrev-expansion nil))

(defvar pabbrev-expand-commands
  '(pabbrev-expand-maybe pabbrev-expand)
  "List of commands which will be used expand.
We need to know this, or the possible expansions are deleted
before the command gets run.")


;; These functions are for displaying a suggestions buffer This bit is
;; in development, and is not meant for end use.  probably will do
;; this as an minor mode I think. Will have to use another
;; post-command-hook to delete the buffer window when it's not appropriate.
;; also think I want to use a arg hook in the
;; pabbrev-post-command-hook, to avoid slowing down the
;; post-command-hook when I am not running.

(defvar pabbrev-suggestions-buffer-enable nil)

(defun pabbrev-suggestions-toggle()
  "NOT FULLY FUNCTIONAL. Enable interactive suggestions window.
This is just a test function at the moment. The idea is that you will
be able to see alternate suggestions as you type. This will be most
useful in a programming buffer. At the moment there is no way of
actually selecting these abbreviations. But it appears that the core
data structures are quick enough to work."
  (interactive)
  (if pabbrev-suggestions-buffer-enable
      (progn
        (setq pabbrev-suggestions-buffer-enable nil)
        (remove-hook 'post-command-hook
                     'pabbrev-suggestions-delete-window)
        (delete-window (get-buffer-window " *pabbrev suggestions*"))
        (message "pabbrev suggestions off"))
    (setq pabbrev-suggestions-buffer-enable t)
    (add-hook 'post-command-hook
              'pabbrev-suggestions-delete-window)
    (message "pabbrev suggestions on")))

(defun pabbrev-suggestions-delete-window()
  (unless
      pabbrev-mode
    (delete-window (get-buffer-window " *pabbrev suggestions*"))))

(defun pabbrev-post-command-delete-suggestions()
  (interactive)
  (if pabbrev-suggestions-buffer-enable
      (progn
        ;; this isn't perfect. The window pops up in a fairly random place.
        (with-output-to-temp-buffer " *pabbrev suggestions*")
        (shrink-window-if-larger-than-buffer (get-buffer-window " *pabbrev suggestions*")))))

(defun pabbrev-post-command-show-suggestions(suggestions)
  (if pabbrev-suggestions-buffer-enable
      (pabbrev-suggestions-buffer suggestions)))


(defun pabbrev-suggestions-buffer(suggestions)
  (with-output-to-temp-buffer " *pabbrev suggestions*"
    (let
        ((window-width (window-width)))
      (save-excursion
        (set-buffer (get-buffer " *pabbrev suggestions*"))
        (princ
         (concat "Max Substring: " (try-completion "" (subseq suggestions 0 10))
                 "\n"))
        (if suggestions
            (dotimes (i 10)
              (if (< i (length suggestions))
                  (progn
                    (goto-char (point-max))
                    (let ((next-suggestion
                           (concat (number-to-string i) " "
                                   (car (nth i suggestions)) " " ))
                          (line-length (- (line-end-position) (line-beginning-position))))
                      ;; if we are not on the first suggestion,
                      (if (and (> i 0)
                               ;; and the line will be too long
                               (< window-width
                                  (+ line-length (length next-suggestion))))
                          ;; add a new line.
                          (setq next-suggestion (concat "\n" next-suggestion)))
                      (princ next-suggestion))))))))
    (shrink-window-if-larger-than-buffer (get-buffer-window " *pabbrev suggestions*"))))


;; These functions define movement around the buffer, which
;; determines what pabbrev considers to be a "word"

(defvar pabbrev-thing-at-point-constituent 'symbol
  "Symbol defining THING which function `pabbrev-mode' works on.
This symbol should be understandable by
`bounds-of-thing-at-point'.  This symbol defines what function `pabbrev-mode'
considers to be the basic unit of expansion.  If if it set to `symbol',
for example, \"pabbrev-mode\" would be offered as an expansion, while
if it is set to `word' \"pabbrev\" and \"mode\" would be offered.
You could also set it to `whitespace' which would be really daft,
or `page' which would be silly in a different way.")

(defun pabbrev-forward-thing(&optional number)
  "Move forward a pabbrev word. Or backwards if number -1"
  (interactive)
  (forward-thing pabbrev-thing-at-point-constituent number))

(defun pabbrev-thing-at-point()
  "Get thing at point."
  (let ((bounds (pabbrev-bounds-of-thing-at-point)))
    (if bounds
        (buffer-substring-no-properties
         (car bounds) (cdr bounds)))))

(defun pabbrev-bounds-of-thing-at-point()
  "Get the bounds of the thing at point"
  (bounds-of-thing-at-point
   pabbrev-thing-at-point-constituent))



;; These functions deal with scavenging word usage from the buffer,
;; which are then added to the dictionary.
(defun pabbrev-bounds-marked-p (start end)
  "Return t if anywhere between START and END is marked."
  (save-excursion
    (let ((retn))
      (do ((i start (1+ i)))
          ((> i end))
        (if
            (setq retn
                  (get-text-property i 'pabbrev-added))
            (setq i end)))
      retn)))


(defun pabbrev-mark-add-word (bounds)
  "Add word in BOUNDS as abbreviation, and mark the buffer."
  (if bounds
      (let ((start (car bounds))
            (end (cdr bounds)))
        (unless
            ;; is this word or part of it already added?
            (pabbrev-bounds-marked-p start end)
          ;; mark the word visibly as well.
          (if pabbrev-debug-display
              (overlay-put
               (make-overlay start end)
               'face 'underline))
          ;; set a property so that we know what we have done.
          (pabbrev-save-buffer-modified-p
           (add-text-properties start end
                                '(pabbrev-added t)))
          ;; and add the word to the system.
          (pabbrev-add-word
           (buffer-substring-no-properties start end))))))

(defvar pabbrev-scavenge-some-chunk-size 40
  "Number of words that `pabbrev-scavenge-words' gathers.
This also affects the speed with which pabbrev will scan through
the buffer during idle, so decrease this if too much processor
is being used, increase it if you want more.  It's set quite
conservatively.  If you get choppy performance when moving
around the buffer you should also consider
`pabbrev-scavenge-on-large-move' to nil.")

(defun pabbrev-scavenge-some()
  "Gather some words up from around point"
  (interactive)
  (save-excursion
    ;; move somewhat away from point, as this is likely to not contain
    ;; complete words.
    (pabbrev-forward-thing -2)
    (pabbrev-scavenge-words -1
                            (* 2 pabbrev-scavenge-some-chunk-size))
    (save-excursion
      (pabbrev-forward-thing 2)
      (pabbrev-scavenge-words 1 pabbrev-scavenge-some-chunk-size))))

(defun pabbrev-scavenge-region()
  (interactive)
  (narrow-to-region (region-beginning) (region-end))
  (pabbrev-scavenge-buffer))
      
(defun pabbrev-scavenge-buffer()
  (interactive)
  (save-excursion
    (goto-char (point-min))
    (while (pabbrev-forward-thing)
      (message "pabbrev scavenging buffer...On line %s"
               (count-lines (point-min) (point)))
      (pabbrev-mark-add-word
       (pabbrev-bounds-of-thing-at-point)))
    (pabbrev-debug-message "Dictionary size %s total usage %s"
                           (pabbrev-get-usage-dictionary-size))
    (message "pabbrev scavenging buffer...done.")))
      

(defun pabbrev-scavenge-words(&optional direction number)
  "Scavenge words from current buffer, starting from point.
DIRECTION is in which direction we should work,
NUMBER is how many words we should try to scavenge"
  (if (not direction)
      (setq direction 1))
  (if (not number)
      (setq number 20))
  (save-excursion
    (dotimes (i number)
      (pabbrev-forward-thing direction)
      (pabbrev-mark-add-word
       (pabbrev-bounds-of-thing-at-point)))
    (point)))


;; switch on the idle timer if required when the mode is switched on.
(add-hook 'pabbrev-mode-on-hook
          'pabbrev-ensure-idle-timer)
;; also run the idle timer function, to put some works in the
;; dictionary.
(add-hook 'pabbrev-mode-on-hook
          'pabbrev-scavenge-some)


(defvar pabbrev-long-idle-timer nil
  "Timer which adds whole buffer.
There are two idle timers which run for function `pabbrev-mode'.  This
one doesn't start for a while, but once it has will work its way
through the whole buffer.  In prints out a message to say what its
doing, and stops on user input.  The variable
`pabbrev-short-idle-timer' is the other.
The idea here is that the short timer will pick up most of the recent
changes, and will not bother the user.  The long timer will slowly
gather up the whole buffer, telling the user what it is doing, in case
it takes up too much processor.  If this happened after a second it
would be irritating in the extreme.")

(defvar pabbrev-short-idle-timer nil
  "Timer which adds a few words.
See `pabbrev-long-idle-timer'.")

(defun pabbrev-ensure-idle-timer()
  (unless nil
    (if (not (and pabbrev-short-idle-timer
                  pabbrev-long-idle-timer))
        (pabbrev-start-idle-timer))))

(defun pabbrev-start-idle-timer()
  (setq pabbrev-long-idle-timer
        (run-with-idle-timer 5 t 'pabbrev-idle-timer-function))
  (setq pabbrev-short-idle-timer
        (run-with-idle-timer 1 t 'pabbrev-short-idle-timer)))

;;(setq  pabbrev-disable-timers t)
(defvar pabbrev-disable-timers nil)

(defun pabbrev-short-idle-timer()
  "Add a few words to the dictionary."
  (if (and pabbrev-mode (not pabbrev-disable-timers))
      (progn
        (pabbrev-debug-message "running short idle timer")
        (pabbrev-scavenge-some)
        (pabbrev-debug-message "Dictionary size %s total usage %s"
                               (pabbrev-get-usage-dictionary-size)
                               (pabbrev-get-total-usages-dictionary)))))

(defun pabbrev-idle-timer-function()
  ;; so this only works on the current buffer. might want to scavenge
  ;; over other buffers
  (if (and pabbrev-mode (not pabbrev-disable-timers))
      (pabbrev-idle-timer-function-0)
    (pabbrev-debug-message "idle running in non pabbrev-mode")))


(defvar pabbrev-idle-timer-verbose t
  "If non NIL, print messages while scavenging on idle timer.

At the moment this is set to t by default.  The idle timer function,
`pabbrev-idle-timer-function' uses quite a bit of processor power, and
I want the users to known what is eating their CPU.  I may change
this at a later date.")

(defun pabbrev-idle-timer-function-0()
  "Add all words to the buffer.
`pabbrev-scavenge-buffer' does this more efficiently interactively.
If this takes up too much processor power, see `pabbrev-scavenge-some-chunk-size'."
  (let ((forward-marker (point))
        (backward-marker (point))
        (forward-complete nil)
        (backward-complete nil)
        (repeat t))
    (message "pabbrev scavenging...")
    (pabbrev-debug-message "running idle timer at %s" (point))
    (while
        (and repeat
             (not (and forward-complete backward-complete)))
      
      (save-excursion
        (unless backward-complete
          (goto-char backward-marker)
          (setq backward-marker
                (pabbrev-scavenge-words -1
                                        (* 2 pabbrev-scavenge-some-chunk-size)))
          (setq backward-complete
                (eq (point-min) backward-marker))
          (pabbrev-debug-message "searching backward to %s complete %s"
                                 backward-marker backward-complete))
        (unless forward-complete
          (goto-char forward-marker)
          (setq forward-marker
                (pabbrev-scavenge-words 1 pabbrev-scavenge-some-chunk-size))
          (setq forward-complete
                (eq (point-max) forward-marker))
          (pabbrev-debug-message "searching forward to %s complete %s"
                                 forward-marker forward-complete)))
      (pabbrev-debug-message "Dictionary size %s total usage %s"
                             (pabbrev-get-usage-dictionary-size)
                             (pabbrev-get-total-usages-dictionary))

      (setq repeat (sit-for 0.1)))
    (message "pabbrev scavenging...done")))


;;; The following are debug functions.

(defvar pabbrev-debug-buffer nil)

;;(setq pabbrev-debug-enabled t)
(defvar pabbrev-debug-enabled nil)

(defun pabbrev-debug-get-buffer()
  (get-buffer-create "*pabbrev-debug"))

(defmacro pabbrev-debug-message(&rest body)
  `(if pabbrev-debug-enabled
       (let ((insert
              (concat (format ,@body) "\n")))
         (save-excursion
           (set-buffer
            (pabbrev-debug-get-buffer))
           (goto-char (point-max))
           (insert insert)
           (pabbrev-debug-frame-scroll)))))


(defun pabbrev-debug()
  (interactive)
  (pabbrev-debug-frame)
  (setq pabbrev-debug-enabled t))

(defvar pabbrev-debug-frame nil)
(defun pabbrev-debug-frame()
  (interactive)
  (if (not pabbrev-debug-frame)
      (progn
        (setq pabbrev-debug-frame
              (make-frame '((width . 30)
                            (height . 30))))
        (select-frame pabbrev-debug-frame)
        (switch-to-buffer (pabbrev-debug-get-buffer)))))

(defun pabbrev-debug-frame-scroll()
  (save-excursion
    (if pabbrev-debug-frame
        (progn
          (select-frame pabbrev-debug-frame)
          (switch-to-buffer (pabbrev-debug-get-buffer))
          (goto-char (point-max))))))

;;(setq pabbrev-debug-display t)
(defvar pabbrev-debug-display nil
  "If t visible mark the progress of function `pabbrev-mode' through the buffer.
This looks very ugly.  Note that this only shows newly added words.  Use
`pabbrev-debug-remove-properties' to clear this invisible markers.  Use
`pabbrev-debug-show-all-properties' to show existing markers.")

(defun pabbrev-debug-erase-all-overlays()
  "Kill all visible overlays from the current buffer. "
  (interactive)
  (pabbrev-debug-remove-properties)
  (mapcar
   (lambda(overlay)
     (if
         (eq 'underline
             (overlay-get overlay 'face))
         (delete-overlay overlay)))
   (overlays-in
    (point-min) (point-max))))


(defun pabbrev-debug-show-all-properties()
  "Show all existing markers.
This can be rather slow."
  (interactive)
  (goto-char (point-min))
  (let ((on-mark-state nil)
        (on-mark))
    (while t
      (progn
        (setq on-mark (get-text-property (point) 'pabbrev-added))
        (message "On line %s"
                 (count-lines (point-min) (point)))
        (cond
         ;; just moved onto marked area
         ((and on-mark (not on-mark-state))
          (setq on-mark-state (point)))
         ;; just moved off a marked area
         ((and on-mark-state (not on-mark))
          (progn
            (overlay-put
             (make-overlay on-mark-state (point))
             'face 'underline)
            (setq on-mark-state nil)))))
      (forward-char))))
                    

(defun pabbrev-debug-restart-idle-timer()
  "Kill and restart the idle timers."
  (interactive)
  (pabbrev-debug-kill-idle-timer)
  (pabbrev-ensure-idle-timer))

(defun pabbrev-debug-kill-idle-timer()
  "Kill the idle timers.
Toggling `pabbrev-mode' will tend to turn them on again, as
will `pabbrev-debug-restart-idle-timer'."
  (interactive)
  (if pabbrev-short-idle-timer
      (progn
        (setq pabbrev-short-idle-timer nil)
        (cancel-timer pabbrev-short-idle-timer)))
  (if pabbrev-long-idle-timer
      (progn
        (setq pabbrev-long-idle-timer)
        (cancel-timer pabbrev-long-idle-timer))))

(defun pabbrev-debug-remove-properties()
  "Remove all the `pabbrev-added' properties from the buffer.
This means all the words in the buffer will be open for addition
to the dictionary."
  (interactive)
  (remove-text-properties
   (point-min)
   (point-max)
   '(pabbrev-added)))

(defun pabbrev-debug-clear-hashes(&optional mode)
  "Clear the dictionary for major mode MODE, or the current mode."
  (interactive)
  (if (not mode)
      (setq mode major-mode))
  (setq pabbrev-prefix-hash-modes
        (delq mode pabbrev-prefix-hash-modes))
  (setq pabbrev-usage-hash-modes
        (delq mode pabbrev-usage-hash-modes))
  ;; help the GC a bit..
  (if (pabbrev-get-usage-hash)
      (progn
        (clrhash (pabbrev-get-usage-hash))
        (put mode 'pabbrev-usage-hash nil)))
  (if (pabbrev-get-prefix-hash)
      (progn
        (clrhash (pabbrev-get-prefix-hash))
        (put mode 'pabbrev-get-prefix-hash nil))))

(defun pabbrev-debug-clear-all-hashes()
  "Clear all hashes for all modes."
  (interactive)
  (mapcar 'pabbrev-debug-clear-hashes pabbrev-prefix-hash-modes))

(defun pabbrev-debug-print-hashes()
  "Print the hashes for the current mode."
  (interactive)
  (let ((usage (pabbrev-get-usage-hash))
        (prefix (pabbrev-get-prefix-hash)))
    (switch-to-buffer
     (get-buffer-create "*pabbrev hash*"))
    (erase-buffer)
    (if (not usage)
        (insert "Usage hash nil"))
    (insert "Usage hash size "
            (number-to-string
             (hash-table-count usage)) "\n")
    (if (not prefix)
        (insert "Prefix hash nil")
      (insert "Prefix hash size "
              (number-to-string
               (hash-table-count prefix)) "\n"))
    (insert "Usage hash:\n")
    (pabbrev-debug-print-hash usage)
    (insert "Prefix hash:\n")
    (pabbrev-debug-print-hash prefix)))

(defun pabbrev-debug-print-hash(hash)
  "Pretty print a hash."
  (if hash
      (progn
        (pp hash (current-buffer))
        (insert "\n")
        (maphash
         (lambda(key value)
           (insert (concat "KEY: " key "\n"))
           (pp value (current-buffer)))
         hash))))



;; 
;; What follows was one of my original ideas for a UI. The idea was
;; that you got offered more than just a single suggestion. I'd still
;; like to do this. The data structures seemed to perform fast
;; enough. The problems are two fold however. The first of which is
;; getting the display to update properly. I tried using the
;; header-line, which doesn't work as to a first approximation point
;; has to move to update, which defeats the
;; point. (force-mode-line-update) does not help. Also I tried using a
;; dedicated frame. Again the display update was slow, although this
;; could probably be overcome.

;; The second problem is the keymap. pabbrev-mode is already fairly
;; intrustive, because its supposed to be quick. I've thought of multi
;; key presses (tab, once to expand, twice to get a mode where you can
;; select numerically from a list.


;; (defvar pabbrev-buffer nil)
;; (defvar pabbrev-frame nil)
;; (defvar pabbrev-cached-frame nil)

;; (defvar pabbrev-frame-parameters '(
;;                                    (width . 30)
;;                                    (height . 30)
;;                                    (border-width . 0)
;;                                    (menu-bar-lines . 0)
;;                                    (tool-bar-lines . 0)
;;                                    (unsplittable . t)))


;; (defun pabbrev-frame(&optional arg)
;;   (interactive "P")
;;   (pabbrev-get-buffer)
;;   (dframe-frame-mode arg
;;                      'pabbrev-frame
;;                      'pabbrev-cached-frame
;;                      'pabbrev-buffer
;;                      "Pabbrev Expand"
;;                      #'pabbrev-minor-mode
;;                      pabbrev-frame-parameters))

;; (defun pabbrev-get-buffer()
;;   (if (not (buffer-live-p pabbrev-buffer))
;;       (setq pabbrev-buffer
;;             (get-buffer-create "*pabbrev expand*")))
;;   pabbrev-buffer)

;; (easy-mmode-define-minor-mode pabbrev-mode
;;                               "Toggle pabbrev mode"
;;                               nil
;;                               " pabbrev"
;;                               '(("\t" . pabbrev-complete-word)
;;                                 ([f1] . (lambda()(interactive) (pabbrev-complete-word 0)))
;;                                 ([f2] . (lambda()(interactive) (pabbrev-complete-word 1)))
;;                                 ([f3] . (lambda()(interactive) (pabbrev-complete-word 2)))
;;                                 ([f4] . (lambda()(interactive) (pabbrev-complete-word 3)))
;;                                 ([f5] . (lambda()(interactive) (pabbrev-complete-word 4)))
;;                                 ([f6] . (lambda()(interactive) (pabbrev-complete-word 5)))
;;                                 ([f7] . (lambda()(interactive) (pabbrev-complete-word 6)))
;;                                 ([f8] . (lambda()(interactive) (pabbrev-complete-word 7)))
;;                                 ([f9] . (lambda()(interactive) (pabbrev-complete-word 8)))
;;                                 ([f10] . (lambda()(interactive) (pabbrev-complete-word 9)))
;;                                 ([f11] . (lambda()(interactive) (pabbrev-complete-word 10)))
;;                                 ([f12] . pabbrev-test-show-word)))

;; (add-hook 'pabbrev-mode-on-hook
;;           'pabbrev-mode-on)
;; (add-hook 'pabbrev-mode-off-hook
;;           'pabbrev-mode-off)


;; (defun pabbrev-mode-on()
;;   (add-hook 'post-command-hook
;;             'pabbrev-post-command-hook t t)
;;   (pabbrev-frame 1))

;; (defun pabbrev-mode-off()
;;   (remove-hook 'post-command-hook 'pabbrev-post-command-hook t)
;;   (pabbrev-frame -1))

;; (defun pabbrev-post-command-hook()
;;   (if (eq last-command 'self-insert-command)
;;       (pabbrev-display-word-before-point)
;;     (pabbrev-not-self-insert-command)))

;; (defun pabbrev-test-show-word()
;;   (interactive)
;;   (pabbrev-display-word-before-point))

;; ;;(setq pabbrev-debug-show-last-command t)

;; (defvar pabbrev-debug-show-last-command nil)

;; (defun pabbrev-not-self-insert-command()
;;   (save-excursion
;;     (setq pabbrev-current-suggestions nil)
;;     (setq pabbrev-current-prefix nil)
;;     (set-buffer (pabbrev-get-buffer))
;;     (erase-buffer)
;;     (if pabbrev-debug-show-last-command
;;         (progn
;;           (insert "Last command was\n" )
;;           (insert (symbol-name last-command))))))

;; (defun pabbrev-display-word-before-point()
;;   (save-excursion
;;     (let ((end (point))
;;            (start
;;             (progn
;;               (backward-word 1)
;;               (point))))
;;       (pabbrev-display-suggestions
;;        (setq pabbrev-current-prefix
;;              (buffer-substring-no-properties start end))
;;        (setq pabbrev-current-suggestions
;;              (pabbrev-fetch-all-suggestions-for-prefix
;;               pabbrev-current-prefix))))))


;; (defvar pabbrev-max-suggestions 12
;;   "This will be the user option" )

;; (defvar pabbrev-total-max-suggestions 12
;;   "This will not be a user option, but will be limited to 12
;; which is the number of function keys")

;; (defvar pabbrev-current-suggestions nil)
;; (defvar pabbrev-current-prefix nil)

;; (defun pabbrev-display-suggestions (prefix suggestions)
;;   (save-excursion
;;     (set-buffer (pabbrev-get-buffer))
;;     (erase-buffer)
;;     (goto-char (point-min))
;;     (if suggestions
;;         (progn
;;           (insert "Suggestions for " prefix "\n")
;;           (dotimes (i
;;                     (min (length suggestions)
;;                          pabbrev-max-suggestions
;;                          pabbrev-total-max-suggestions))
;;             (insert "f" (number-to-string (+ 1 i)) ": "
;;                     (car (nth i suggestions)) "\n" )))
;;       (insert "No suggestions" ))
;;     (select-frame pabbrev-frame)
;;     (sit-for 0)))

;; (defun pabbrev-complete-word (&optional suggestion)
;;   (interactive)
;;   ;; default value
;;   (unless suggestion
;;     (setq suggestion 0))
;;   ;; no prefix, can not complete
;;   (if (and pabbrev-current-prefix
;;            (< suggestion (length
;;                            pabbrev-current-suggestions)))
;;       (let ((string
;;              (car (nth suggestion pabbrev-current-suggestions))))
;;         ;; should be in the right buffer, right place
;;         (insert
;;          ;; chop of the prefix
;;          (substring
;;           string
;;           (length pabbrev-current-prefix)))
;;         (message "Completed word to %s" string))
;;     (message "No appropriate completion")))


(provide 'pabbrev)

;;; pabbrev.el ends here

