(load (concat (getenv "EMACSRC") "/emacs-d/d-mode.el"))

(setq indent-tabs-mode nil)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;key bindings
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(global-unset-key "\M-r")
(global-set-key "\M-rp" 'd-insert-puts)
(global-set-key "\M-ri" 'd-insert-import)
;; (global-set-key "\M-ry" 'cc-embed-expression)
;; (global-set-key "\M-re" 'cc-for-loop)
;; (global-set-key "\M-ri" 'cc-if)
;; (global-set-key "\M-rm" 'cc-move-defs)
;; (global-set-key "\M-rc" 'cc-insert-class)
;; (global-set-key "\M-rr" 'cc-restore-gdb)
;; (global-set-key "\M-rw" 'cc-widen-gdb)

(defun d-write-to-buffer (par)
  ""
  (princ par (current-buffer))
  )

(defun d-insert-writefln()
  (interactive)
  (d-write-to-buffer "writefln(\"\");")
  (backward-char 3)
  (indent-according-to-mode)
  )

(defun d-insert-Stdout()
  (interactive)
  (d-write-to-buffer "Stdout.formatln(\"\");")
  (backward-char 3)
  (indent-according-to-mode)
  )

(defun d-insert-puts()
  (interactive)
  (d-write-to-buffer "puts(\"\");")
  (backward-char 3)
  (indent-according-to-mode)
  )

(defun d-insert-import(moduleName)
  (interactive "MModule to import: ")
  (d-write-to-buffer "import ")
  (d-write-to-buffer moduleName)
  (d-write-to-buffer ";\n")
  )

;; (defun cc-embed-expression()
;;   (interactive)
;;   (d-write-to-buffer "\" <<  << \"")
;;   (backward-char 5)
;;   (indent-according-to-mode)
;;   )

;; (defun cc-for-loop(iteratorName vectorName)
;;   (interactive "MIterator: \nMVector: ")
;;   (d-write-to-buffer "for (size_t ")
;;   (d-write-to-buffer iteratorName)
;;   (d-write-to-buffer "=0; ")
;;   (d-write-to-buffer iteratorName)
;;   (d-write-to-buffer "<")
;;   (d-write-to-buffer vectorName)
;;   (d-write-to-buffer ".size(); ++")
;;   (d-write-to-buffer iteratorName)
;;   (d-write-to-buffer ")")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n{")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n\n}")
;;   (indent-according-to-mode)
;;   (forward-line -1)
;;   (indent-according-to-mode)
;;   )

;; (defun cc-if(condition)
;;   (interactive "MCondition: ")
;;   (d-write-to-buffer "if (")
;;   (d-write-to-buffer condition)
;;   (d-write-to-buffer ")")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n{")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n\n}")
;;   (indent-according-to-mode)
;;   (forward-line -1)
;;   (indent-according-to-mode)
;;   )

;; (defun cc-insert-class(className super)
;;   (interactive "MClass name: \nMBase class: ")
;;   (d-write-to-buffer "class ")
;;   (d-write-to-buffer className)
;;   (if (not (string= super ""))
;;       (d-write-to-buffer (with-output-to-string (princ ": public ") (princ super)))
;;     )
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n{")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\npublic:")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n")
;;   (d-write-to-buffer className)
;;   (d-write-to-buffer "() {};")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\nprivate:")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n")
;;   (indent-according-to-mode)
;;   (d-write-to-buffer "\n};")
;;   (indent-according-to-mode)
;;   (forward-line -3)
;;   )

;; (defun cc-move-defs()
;;   (interactive)
;;   (kill-line 1)
;;   (next-line)
;;   (yank)
;;   (kill-line 1)
;;   (next-line)
;;   (yank)
;;   (previous-line)
;;   (previous-line)
;;   (previous-line)
;;   )

;; (defun cc-restore-gdb()
;;   (interactive)
;;   (gdb-restore-windows)
;;   )

;; (defun cc-widen-gdb()
;;   (interactive)
;;   (enlarge-window-horizontally 180)
;;   )

;; ;(global-set-key "\M-rE" 'ruby-electric-mode)
;; ;(global-set-key "\M-rP" 'pabbrev-mode)
;; ;(global-unset-key "\C-o")
;; ;(global-set-key "\C-o" 'open-new-line)
;; ;
;; ;(global-set-key "\M-rc" 'ruby-insert-class)
;; ;(global-set-key "\M-rf" 'ruby-add-method-to-class)
;; ;
;; ;(global-set-key "\M-rm" 'ruby-insert-method)
;; ;(global-set-key "\M-ra" 'ruby-insert-array-hash-element)
;; ;
;; ;(global-set-key "\M-rn" 'ruby-insert-new-object)
;; ;
;; ;;block (small,large)
;; ;(global-set-key "\M-ri" 'ruby-insert-small-block)
;; ;(global-set-key "\M-ro" 'ruby-insert-big-block)
;; ;;method with block (small,large)
;; ;(global-set-key "\M-rj" 'ruby-insert-method-small-block)
;; ;(global-set-key "\M-rk" 'ruby-insert-method-big-block)
;; ;
;; ;;each (large,small)
;; ;(global-set-key "\M-re" 'ruby-insert-each-big-block)
;; ;(global-set-key "\M-rw" 'ruby-insert-each-small-block)
;; ;;each_with_index (large,small)
;; ;(global-set-key "\M-rd" 'ruby-insert-each-with-index-big-block)
;; ;(global-set-key "\M-rs" 'ruby-insert-each-with-index-small-block)
;; ;
;; ;;collect (large,small)
;; ;(global-set-key "\M-rx" 'ruby-insert-collect-big-block)
;; ;(global-set-key "\M-rz" 'ruby-insert-collect-small-block)
;; ;
;; ;
;; ;;embed
;; ;(global-set-key "\M-ry" 'ruby-embed-expression)
;; ;
;; ;;require
;; ;(global-set-key "\M-rr" 'ruby-insert-require)
;; ;
;; ;;rhtml
;; ;(global-set-key "\M-rt" 'ruby-insert-html)
;; ;(global-set-key "\M-rg" 'ruby-insert-html-output)
;; ;
;; ;;hash element
;; ;(global-set-key "\M-rh" 'ruby-insert-hash-element)
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ;;;interactive functions
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ;(defun ruby-insert-each-big-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for each: ")
;; ;  (ruby-insert-method-block "each" "" "big" blockPars)
;; ;  )
;; ;(defun ruby-insert-each-small-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for each: ")
;; ;  (ruby-insert-method-block "each" "" "small" blockPars)
;; ;  )
;; ;
;; ;(defun ruby-insert-each-with-index-big-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for each: ")
;; ;  (ruby-insert-method-block "each_with_index" "" "big" blockPars)
;; ;  )
;; ;(defun ruby-insert-each-with-index-small-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for each: ")
;; ;  (ruby-insert-method-block "each_with_index" "" "small" blockPars)
;; ;  )
;; ;
;; ;(defun ruby-insert-collect-big-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for collect: ")
;; ;  (ruby-insert-method-block "collect" "" "big" blockPars)
;; ;  )
;; ;(defun ruby-insert-collect-small-block (blockPars)
;; ;  "ruby-insert-big-each"
;; ;  (interactive "MBlock parameters for collect: ")
;; ;  (ruby-insert-method-block "collect" "" "small" blockPars)
;; ;  )
;; ;
;; ;(defun ruby-insert-method-big-block (methodName methodArgs blockPars)
;; ;  ""
;; ;  (interactive "MMethod name: \nMMethod arguments: \nMBlock parameters: ")
;; ;  (ruby-insert-method-block methodName methodArgs "big" blockPars)
;; ;  )
;; ;(defun ruby-insert-method-small-block (methodName methodArgs blockPars)
;; ;  ""
;; ;  (interactive "MMethod name: \nMMethod arguments: \nMBlock parameters: ")
;; ;  (ruby-insert-method-block methodName methodArgs "small" blockPars)
;; ;  )
;; ;
;; ;(defun ruby-insert-big-block (blockPars)
;; ;  ""
;; ;  (interactive "MBlock parameters: ")
;; ;  (ruby-insert-block "big" blockPars)
;; ;  )
;; ;(defun ruby-insert-small-block (blockPars)
;; ;  ""
;; ;  (interactive "MBlock parameters: ")
;; ;  (ruby-insert-block "small" blockPars)
;; ;  )
;; ;
;; ;(defun ruby-insert-method (methodName methodArgs)
;; ;  (interactive "MMethod name: \nMMethod arguments: ")
;; ;  (ruby-insert-dot)
;; ;  (ruby-write-to-buffer methodName)
;; ;  (if (not (string= methodArgs ""))
;; ;      (ruby-write-to-buffer (with-output-to-string (princ "(") (princ methodArgs) (princ ")")))
;; ;    )
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-hash-element (before after)
;; ;  (interactive "MBefore: \nMAfter: ")
;; ;  (ruby-write-to-buffer before)
;; ;  (ruby-write-to-buffer " => ")
;; ;  (ruby-write-to-buffer after)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-array-hash-element (AryHashName AryHashArgs)
;; ;  (interactive "MArray or hash name: \nMArray or hash index: ")
;; ;  (ruby-insert-dot)
;; ;  (ruby-write-to-buffer AryHashName)
;; ;  (if (not (string= AryHashArgs ""))
;; ;      (ruby-write-to-buffer (with-output-to-string (princ "[") (princ AryHashArgs) (princ "]")))
;; ;    )
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-new-object (objectType objectArgs)
;; ;  (interactive "MObject class: \nMObject arguments: ")
;; ;  (ruby-write-to-buffer objectType)
;; ;  (ruby-write-to-buffer ".new")
;; ;  (if (not (string= objectArgs ""))
;; ;      (ruby-write-to-buffer (with-output-to-string (princ "(") (princ objectArgs) (princ ")")))
;; ;    )
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-class (className super)
;; ;  (interactive "MClass name: \nMSuper class: ")
;; ;  (ruby-write-to-buffer "class ")
;; ;  (ruby-write-to-buffer className)
;; ;  (if (not (string= super ""))
;; ;      (ruby-write-to-buffer (with-output-to-string (princ " < ") (princ super)))
;; ;    )
;; ;  (ruby-write-to-buffer "\ndef initialize()")
;; ;  (indent-according-to-mode)
;; ;  (ruby-write-to-buffer "\n\nend")
;; ;  (indent-according-to-mode)
;; ;  (ruby-write-to-buffer "\nend")
;; ;  (indent-according-to-mode)
;; ;  (forward-line -2)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-add-method-to-class(methodName methodArgs)
;; ;  (interactive "MMethod name: \nMMethod arguments: ")
;; ;  (ruby-write-to-buffer "def ")
;; ;  (ruby-write-to-buffer methodName)
;; ;  (if (not (string= methodArgs ""))
;; ;      (ruby-write-to-buffer (with-output-to-string (princ "(") (princ methodArgs) (princ ")")))
;; ;    )
;; ;  (indent-according-to-mode)
;; ;  (ruby-write-to-buffer "\n\nend")
;; ;  (indent-according-to-mode)
;; ;  (forward-line -1)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-html()
;; ;  (interactive)
;; ;  (ruby-write-to-buffer "<%  %>")
;; ;  (backward-char 3)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-html-output()
;; ;  (interactive)
;; ;  (ruby-write-to-buffer "<%=  %>")
;; ;  (backward-char 3)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-embed-expression (str)
;; ;  (interactive "MExpression to embed: ")
;; ;  (ruby-write-to-buffer "#{")
;; ;  (ruby-write-to-buffer str)
;; ;  (ruby-write-to-buffer "}")
;; ;;  (backward-char 1)
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-require (str)
;; ;  (interactive "MModule to require: ")
;; ;  (ruby-write-to-buffer "require(\"")
;; ;  (ruby-write-to-buffer str)
;; ;  (ruby-write-to-buffer "\")\n")
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ;;internal functions
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; (defun d-write-to-buffer (par)
;;   ""
;;   (princ par (current-buffer))
;;   )

;; ;(defun ruby-insert-dot ()
;; ;  (backward-char)
;; ;  (cond
;; ;   ((looking-at "[0-9|a-z|A-Z)]")
;; ;    (forward-char)
;; ;    (ruby-write-to-buffer ".")
;; ;    )
;; ;   (t
;; ;    (forward-char)
;; ;    )
;; ;   )
;; ;  )
;; ;
;; ;(defun ruby-insert-block(blockType blockPars)
;; ;  "ruby-insert-big-block"
;; ;
;; ;    (
;; ;     )
;; ;  (cond
;; ;   ((string= blockType "big") 
;; ;    (ruby-write-to-buffer (with-output-to-string
;; ;			    (princ " do |")
;; ;			    (princ blockPars)
;; ;			    (princ "|")))
;; ;    (indent-according-to-mode)
;; ;    (ruby-write-to-buffer "\n\nend")
;; ;    (indent-according-to-mode)
;; ;    (forward-line -1)
;; ;    (indent-according-to-mode)
;; ;    )
;; ;   ((string= blockType "small")
;; ;    (ruby-write-to-buffer (with-output-to-string
;; ;			    (princ "{|")
;; ;			    (princ blockPars)
;; ;			    (princ "|}")))
;; ;    (backward-char)
;; ;    )
;; ;   )
;; ;  (indent-according-to-mode)
;; ;  )
;; ;
;; ;(defun ruby-insert-method-block (methodName methodArgs blockType blockPars)
;; ;  ""
;; ;  (ruby-insert-dot)
;; ;  (princ methodName (current-buffer))
;; ;  (if (not(string= methodArgs ""))
;; ;      (ruby-write-to-buffer (with-output-to-string
;; ;	       (princ "(")
;; ;	       (princ methodArgs)
;; ;	       (princ ")")
;; ;	       ))
;; ;    )
;; ;  (ruby-insert-block blockType blockPars)
;; ;  )
;; ;
;; ;(defun open-new-line ()
;; ;  (interactive)
;; ;  (end-of-line)
;; ;  (ruby-write-to-buffer "\n")
;; ;  (indent-according-to-mode)
;; ;  )
