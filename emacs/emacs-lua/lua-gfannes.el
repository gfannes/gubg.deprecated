;this defines the color stuff from lua
(load (concat (getenv "EMACSRC") "/emacs-lua/lua-mode.el"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;key bindings
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(global-unset-key "\M-l")

(global-set-key "\M-lf" 'lua-insert-new-function)
(global-set-key "\M-lF" 'lua-insert-function-call)
(global-set-key "\M-ll" 'lua-insert-local)

(global-set-key "\M-la" 'lua-insert-array-hash-element)

;for
(global-set-key "\M-le" 'lua-insert-for-pairs)
(global-set-key "\M-lE" 'lua-insert-for-ipairs)
(global-set-key "\M-lg" 'lua-insert-for-generic)

;if
(global-set-key "\M-li" 'lua-insert-if)
(global-set-key "\M-lI" 'lua-insert-elseif)

;print
(global-set-key "\M-lp" 'lua-insert-print)

;embed
(global-set-key "\M-ly" 'lua-embed-expression)

;require
(global-set-key "\M-lr" 'lua-insert-require)

;hash element
(global-set-key "\M-lh" 'lua-insert-hash-element)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;interactive functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun lua-insert-each-big-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for each: ")
  (lua-insert-method-block "each" "" "big" blockPars)
  )
(defun lua-insert-each-small-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for each: ")
  (lua-insert-method-block "each" "" "small" blockPars)
  )

(defun lua-insert-each-with-index-big-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for each: ")
  (lua-insert-method-block "each_with_index" "" "big" blockPars)
  )
(defun lua-insert-each-with-index-small-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for each: ")
  (lua-insert-method-block "each_with_index" "" "small" blockPars)
  )

(defun lua-insert-collect-big-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for collect: ")
  (lua-insert-method-block "collect" "" "big" blockPars)
  )
(defun lua-insert-collect-small-block (blockPars)
  "lua-insert-big-each"
  (interactive "MBlock parameters for collect: ")
  (lua-insert-method-block "collect" "" "small" blockPars)
  )

(defun lua-insert-method-big-block (methodName methodArgs blockPars)
  ""
  (interactive "MMethod name: \nMMethod arguments: \nMBlock parameters: ")
  (lua-insert-method-block methodName methodArgs "big" blockPars)
  )
(defun lua-insert-method-small-block (methodName methodArgs blockPars)
  ""
  (interactive "MMethod name: \nMMethod arguments: \nMBlock parameters: ")
  (lua-insert-method-block methodName methodArgs "small" blockPars)
  )

(defun lua-insert-big-block (blockPars)
  ""
  (interactive "MBlock parameters: ")
  (lua-insert-block "big" blockPars)
  )
(defun lua-insert-small-block (blockPars)
  ""
  (interactive "MBlock parameters: ")
  (lua-insert-block "small" blockPars)
  )

(defun lua-insert-hash-element (before after)
  (interactive "MBefore: \nMAfter: ")
  (lua-write-to-buffer before)
  (lua-write-to-buffer " => ")
  (lua-write-to-buffer after)
  (indent-according-to-mode)
  )

(defun lua-insert-array-hash-element (AryHashName AryHashArgs)
  (interactive "MArray or hash name: \nMArray or hash index: ")
  (lua-insert-dot)
  (lua-write-to-buffer AryHashName)
  (if (not (string= AryHashArgs ""))
      (lua-write-to-buffer (with-output-to-string (princ "[") (princ AryHashArgs) (princ "]")))
    )
  (indent-according-to-mode)
  )

(defun lua-insert-new-function(functionName functionArgs)
  (interactive "MFunction name: \nMFunction arguments: ")
  (lua-write-to-buffer "function ")
  (lua-write-to-buffer functionName)
  (lua-write-to-buffer (with-output-to-string (princ "(") (princ functionArgs) (princ ")")))
  (indent-according-to-mode)
  (lua-write-to-buffer "\n\nend")
  (indent-according-to-mode)
  (forward-line -1)
  (indent-according-to-mode)
  )

(defun lua-insert-function-call(functionName)
  (interactive "MFunction name: ")
  (lua-write-to-buffer functionName)
  (lua-write-to-buffer "()")
  (backward-char 1)
  (indent-according-to-mode)
  )

(defun lua-insert-local()
  (interactive)
  (lua-write-to-buffer "local ")
  (indent-according-to-mode)
  )

(defun lua-insert-for-pairs(forValues tableName)
  (interactive "MFor values: \nMTable: ")
  (lua-write-to-buffer "for ")
  (lua-write-to-buffer forValues)
  (lua-write-to-buffer " in pairs(")
  (lua-write-to-buffer tableName)
  (lua-write-to-buffer ") do")
  (indent-according-to-mode)
  (lua-write-to-buffer "\n\nend")
  (indent-according-to-mode)
  (forward-line -1)
  (indent-according-to-mode)
  )

(defun lua-insert-for-ipairs(forValues tableName)
  (interactive "MFor values: \nMTable: ")
  (lua-write-to-buffer "for ")
  (lua-write-to-buffer forValues)
  (lua-write-to-buffer " in ipairs(")
  (lua-write-to-buffer tableName)
  (lua-write-to-buffer ") do")
  (indent-according-to-mode)
  (lua-write-to-buffer "\n\nend")
  (indent-according-to-mode)
  (forward-line -1)
  (indent-according-to-mode)
  )

(defun lua-insert-for-generic(forValues iterFactory)
  (interactive "MFor values: \nMFor iterator factory: ")
  (lua-write-to-buffer "for ")
  (lua-write-to-buffer forValues)
  (lua-write-to-buffer " in ")
  (lua-write-to-buffer iterFactory)
  (lua-write-to-buffer " do")
  (indent-according-to-mode)
  (lua-write-to-buffer "\n\nend")
  (indent-according-to-mode)
  (forward-line -1)
  (indent-according-to-mode)
  )

(defun lua-insert-if(condition)
  (interactive "MCondition: ")
  (lua-write-to-buffer "if ")
  (lua-write-to-buffer condition)
  (lua-write-to-buffer " then")
  (indent-according-to-mode)
  (lua-write-to-buffer "\n\nend")
  (indent-according-to-mode)
  (forward-line -1)
  (indent-according-to-mode)
  )

(defun lua-insert-elseif(condition)
  (interactive "MCondition: ")
  (lua-write-to-buffer "elseif ")
  (lua-write-to-buffer condition)
  (lua-write-to-buffer " then")
  (indent-according-to-mode)
  (lua-write-to-buffer "\n")
  (indent-according-to-mode)
  )

(defun lua-insert-print()
  (interactive)
  (lua-insert-dot)
  (lua-write-to-buffer "print(\"\")")
  (backward-char 2)
  (indent-according-to-mode)
  )

(defun lua-embed-expression (str)
  (interactive "MExpression to embed: ")
  (lua-write-to-buffer "\" .. ")
  (lua-write-to-buffer str)
  (lua-write-to-buffer " .. \"")
;  (backward-char 1)
  (indent-according-to-mode)
  )

(defun lua-insert-require (str)
  (interactive "MModule to require: ")
  (lua-write-to-buffer "require(\"")
  (lua-write-to-buffer str)
  (lua-write-to-buffer "\")\n")
  (indent-according-to-mode)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;internal functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun lua-insert-dot ()
  (backward-char)
  (cond
   ((looking-at "[0-9|a-z|A-Z)]")
    (forward-char)
    (lua-write-to-buffer ".")
    )
   (t
    (forward-char)
    )
   )
  )

(defun lua-write-to-buffer (par)
  ""
  (princ par (current-buffer))
  )

(defun lua-insert-block(blockType blockPars)
  "lua-insert-big-block"

    (
     )
  (cond
   ((string= blockType "big") 
    (lua-write-to-buffer (with-output-to-string
			    (princ " do |")
			    (princ blockPars)
			    (princ "|")))
    (indent-according-to-mode)
    (lua-write-to-buffer "\n\nend")
    (indent-according-to-mode)
    (forward-line -1)
    (indent-according-to-mode)
    )
   ((string= blockType "small")
    (lua-write-to-buffer (with-output-to-string
			    (princ "{|")
			    (princ blockPars)
			    (princ "|}")))
    (backward-char)
    )
   )
  (indent-according-to-mode)
  )

(defun lua-insert-method-block (methodName methodArgs blockType blockPars)
  ""
  (lua-insert-dot)
  (princ methodName (current-buffer))
  (if (not(string= methodArgs ""))
      (lua-write-to-buffer (with-output-to-string
	       (princ "(")
	       (princ methodArgs)
	       (princ ")")
	       ))
    )
  (lua-insert-block blockType blockPars)
  )

(defun open-new-line ()
  (interactive)
  (end-of-line)
  (lua-write-to-buffer "\n")
  (indent-according-to-mode)
  )
