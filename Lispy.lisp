(defconstant false nil)
(defconstant true t)
(defconstant *white-space* '(#\Newline #\Space #\Tab))


(setf (fdefinition 'new-line) #'terpri)


(defmacro alambda (parameters &body body)
  `(labels ((self ,parameters ,@body))
     #'self))


(defmethod bind ((fn function) &rest bound-arguments)
  (lambda (&rest arguments)
    (apply fn (append bound-arguments arguments))))


(defmethod clone ((n number))
  n)


(defmethod clone ((seq sequence))
  (copy-seq seq))


(defmethod clone ((sym symbol))
  sym)


(defmethod compose (&rest functions)
  (if (empty? functions)
      #'identity
      (let ((fn (first (last functions)))
            (rest (butlast functions)))
        
        (lambda (&rest arguments)
          (reduce #'funcall
                  rest
                  :from-end true
                  :initial-value (apply fn arguments))))))


(defmacro define-class (name &optional (parents '()) (slots '()))
  (labels ((create-argument (slot)
             `(,(slot-arg slot) ,(slot-name slot)))
           
           (create-parameter (slot)
             `(,(slot-name slot) ,(slot-value slot)))
           
           (create-slot (slot)
             (let ((name (slot-name slot)))
               `(,name :accessor ,(slot-accessor slot)
                       :initarg ,(slot-arg slot))))
           
           (slot-accessor (slot)
             (read-from-string (format nil "~A-of" (slot-name slot))))
           
           (slot-arg (slot)
             (read-from-string (format nil ":~A" (slot-name slot))))
           
           (slot-name (slot)
             (if (listp slot)
                 (first slot)
                 slot))
           
           (slot-value (slot)
             (if (listp slot)
                 (second slot)
                 nil)))
    
    `(prog1
       (defclass ,name ,parents
         ,(mapcar #'create-slot slots))
       
       (defmethod ,name (&key ,@(mapcar #'create-parameter slots))
         (make-instance ',name ,@(mapcan #'create-argument slots)))
       
       (defmethod clone ((object ,name))
         (,name ,@(mapcan (lambda (slot)
                            `(,(slot-arg slot) (clone (,(slot-accessor slot) object))))
                          slots)))
       
       (defmethod equal? ((x ,name) (y ,name))
         (and ,@(mapcar (lambda (slot)
                          `(equal? (,(slot-accessor slot) x)
                                   (,(slot-accessor slot) y)))
                        slots))))))


(defmethod equal? (x y)
  (equalp x y))


(defmethod equal? ((x string) (y string))
  (equal x y))


(defmethod empty? ((l list))
  (null l))


(defmethod empty? ((str string))
  (zerop (length str)))


; Does the same as the deprecated function "remove-if-not".
(defmethod filter ((include? function) (values list))
  (if (empty? values)
      ()
      (let ((value (first values)))
        (if (funcall include? value)
            (cons value (filter include? (rest values)))
            (filter include? (rest values))))))


(defmethod flatten ((values list))
  (cond ((empty? values)
         ())
        
        ((listp (first values))
         (append (flatten (first values))
                 (flatten (rest values))))
        
        (t (cons (first values)
                 (flatten (rest values))))))


(defmethod format-size ((size integer))
  (let ((units #("B" "KiB" "MiB" "GiB" "TiB" "PiB" "EiB" "ZiB" "YiB")))
    (do ((unit 0 (1+ unit)))
        ((< size 1024) (format nil "~,2F ~A" size (aref units unit)))
      (setf size (/ size 1024)))))


(defmacro get-current-macro-environment (&environment env)
  env)


(defmethod join ((values list) &optional (separator ""))
  (reduce (bind #'concatenate 'string)
          (rest (mapcan (compose (bind #'list (princ-to-string separator))
                                 #'princ-to-string)
                        values))))


(defmethod leap-year? ((year integer))
  (or (zerop (rem year 400))
      (and (zerop (rem year 4))
           (not (zerop (rem year 100))))))


(defmacro macro (name &environment env)
  "Converts a macro to a function, e.g. (funcall (macro and) 1 2) --> 2"
  #-CLISP (error "Only CLISP is supported.")
  
  (let ((local-env (gensym))
        (macro-env (gensym)))
    
    `(let ((,local-env (ext:the-environment))
           (,macro-env ,env))
       
       (lambda (&rest arguments)
         (ext:eval-env (funcall (macro-function ',name ,macro-env)
                                (cons ',name arguments)
                                ,macro-env)
                       ,local-env)))))


(define-symbol-macro macro-environment
  (get-current-macro-environment))


(defmethod memorize ((fn function))
  (let ((cache (make-hash-table :test #'equal?)))
    
    (lambda (&rest arguments)
      (multiple-value-bind (value found?) (gethash arguments cache)
        
        (if found?
            value
            (setf (gethash arguments cache)
                  (apply fn arguments)))))))


(defmethod next-tab-column ((column integer) &key (step 8))
  (- (+ column step)
     (rem column step)))


(defmethod range ((count integer) &key (start 0) (step 1))
  (loop repeat count for i from start by step collect i))


(defmethod read-C-string ((st stream))
  (let ((chars '()))
    (do ((b (read-byte st) (read-byte st)))
        ((= b (char-code #\Null)) (coerce (reverse chars) 'string))
      (push (code-char b) chars))))


(defmethod read-int ((st stream) (size integer) (format function))
  (let ((bytes '()))
    (dotimes (i size)
      (push (read-byte st) bytes))
    (funcall format (reverse bytes))))


(defmethod read-int ((vec vector) (size integer) (format function))
  (let ((bytes '()))
    (dotimes (i size)
      (push (vector-pop vec) bytes))
    (funcall format bytes)))


(defmethod read-int32 (place (format function))
  (read-int place 4 format))


(shadow 'reverse)

(defmethod reverse ((n integer))
  (values (read-from-string (reverse (princ-to-string n)))))


(defmethod reverse ((seq sequence))
  (CL:reverse seq))


(defmethod split ((str string))
  (loop for char across str collect char))


(defmethod trim ((str string))
  (string-trim *white-space* str))


(defmacro until (condition &body body)
  `(while (not ,condition)
     ,@body))


(defmacro while (condition &body body)
  `(do ()
       ((not ,condition))
     ,@body))


(unless (equalp *package* (symbol-package 'with-gensyms))
  (shadow 'with-gensyms))

(defmacro with-gensyms (symbols &body body)
  `(let ,(mapcar (lambda (symbol) `(,symbol (gensym)))
                 symbols)
     ,@body))


(defmethod write-C-string ((st stream) (str string))
  (dotimes (i (length str))
    (write-byte (char-code (char str i)) st))
  (write-byte (char-code #\Null) st))


(defmethod write-int ((st stream) (size integer) (format function) (n integer))
  (dolist (b (funcall format n size))
    (write-byte b st)))


(defmethod write-int ((vec vector) (size integer) (format function) (n integer))
  (dolist (b (funcall format n size))
    (vector-push b vec)))


(defmethod write-int32 (place (format function) (n integer))
  (write-int place 4 format n))


(defun Big-Endian (n &optional size)
  (if (listp n)
      (if (empty? n)
          0
          (let ((high (ash (first n) (* (- (length n) 1) 8)))
                (low (Big-Endian (rest n))))
            (logior high low)))
      
      (let ((bytes '()))
        (do ((i n (ash i -8)))
            ((and (zerop i) (not (empty? bytes))))
          (push (mask-field (byte 8 0) i) bytes))
        
        (if size
            (let ((len (length bytes)))
              (cond ((> len size)
                     (nthcdr (- len size) bytes))
                    
                    ((< len size)
                     (let ((padding '()))
                       (dotimes (i (- size len))
                         (push 0 padding))
                       (append padding bytes)))
                    
                    (t bytes)))
            bytes))))


(defun Little-Endian (n &optional size)
  (if (listp n)
      (Big-Endian (reverse n) size)
      (reverse (Big-Endian n size))))


(defmacro XML (&body tags)
  (labels ((compile (tags)
             (if (empty? tags)
                 '()
                 `(progn ,(compile-expression (first tags))
                         ,(compile (rest tags)))))
           
           (compile-attribute (attribute)
             (let ((key `(princ ,(first attribute)))
                   (value (second attribute)))
               
               (if (atom value)
                   (setf value `(princ ,value)))
               
               `(progn ,key (princ "=\"") ,value (princ "\""))))
           
           (compile-expression (expression)
             (cond ((pathnamep expression)
                    (with-gensyms (file char)
                      `(with-open-file (,file ,expression)
                         (do ((,char (read-char ,file nil 'eof)
                                     (read-char ,file nil 'eof)))
                             ((equal? ,char 'eof))
                           (princ ,char)))))
                   
                   ((atom expression)
                    `(princ ,expression))
                   
                   ((keywordp (first expression))
                    (compile-tag expression))
                   
                   (t expression)))
           
           (compile-tag (tag)
             (let* ((tag-name (first tag))
                    (attributes ())
                    (content (rest tag)))
               
               (when (and (listp (second tag))
                          (equal? (first (second tag)) '@))
                 (setf attributes (mapcar #'compile-attribute (rest (first content))))
                 (setf content (rest content)))
               
               `(progn
                  (princ ,(format nil "<~A" tag-name))
                  ,@(mapcan (bind #'list '(princ " ")) attributes)
                  (princ ">")
                  ,(compile content)
                  (princ ,(format nil "</~A>" tag-name))))))
  
  `(macrolet ((? (function &rest arguments)
                `(princ (,function ,@arguments))))
     
     ,(compile tags))))
