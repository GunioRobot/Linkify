(load "Lispy.lisp")


(defun compute-prefix-table (pattern)
  (let ((prefix (make-array (length pattern) :initial-element 0))
        (k 0))
    
    (do ((q 1 (1+ q)))
        ((>= q (length pattern)) prefix)
      
      (loop while (and (> k 0)
                       (not (equal? (nth k pattern) (nth q pattern))))
            do (setf k (aref prefix (1- k))))
      
      (when (equal? (nth k pattern) (nth q pattern))
        (incf k))
      
      (setf (aref prefix q) k))))


(defun matcher (pattern text)
  (let ((prefix (compute-prefix-table pattern))
        (offsets ())
        (q 0))
    
    (dotimes (i (length text) (reverse offsets))
      (loop while (and (> q 0)
                       (not (equal? (nth q pattern) (nth i text))))
            do (setf q (aref prefix (1- q))))
      
      (when (equal? (nth q pattern) (nth i text))
        (incf q))
      
      (when (= q (length pattern))
        (push (- (1+ i) (length pattern)) offsets)
        (setf q (aref prefix (1- q)))))))


; Test.
(let ((pattern-1 (split "aaaababbaaba"))
      (pattern-2 (split "abaa"))
      (text-2 (split "aababaaaa")))
  
  (assert (equal? #1A(0 1 2 3 0 1 0 0 1 2 0 1) (compute-prefix-table pattern-1)))
  (assert (equal? (list 3) (matcher pattern-2 text-2))))


; Start.
(let* ((pattern (progn (princ "Pattern: ")
                       (split (trim (read-line)))))
       (text (progn (princ "Text, empty to skip: ")
                    (split (trim (read-line)))))
       (prefix (compute-prefix-table pattern)))
  
  (format t "~%Position:")
  (dotimes (i (length pattern)) (format t " ~A" (1+ i)))
  (new-line)
  
  (princ " Pattern:")
  (dolist (char pattern) (format t " ~A" char))
  (new-line)
  
  (princ "  Prefix:")
  (dotimes (i (length pattern)) (format t " ~A" (aref prefix i)))
  (new-line)
  
  (unless (empty? text)
    (let ((offsets (matcher pattern text)))
      
      (format t "~%Position:")
      (dotimes (i (length text)) (format t " ~A" (1+ i)))
      (new-line)
      
      (princ "    Text:")
      (dolist (char text) (format t " ~A" char))
      (new-line)
      
      (princ " Offsets:")
      
      (if (empty? offsets)
          (princ " (None.)")
          (dotimes (i (length text))
            (cond ((= i (first offsets))
                   (princ " ^")
                   (pop offsets)
                   (when (empty? offsets) (return)))
                  (t (princ "  ")))))
      
      (new-line))))
