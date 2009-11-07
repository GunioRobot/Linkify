(load "Lispy.lisp")


(defun compute-matrix (seq-S seq-T)
  (let* ((rows (length seq-S))
         (columns (length seq-T))
         (matrix-C (make-array (list rows columns) :initial-element 0)))
    
    (flet ((at (row column)
             (if (array-in-bounds-p matrix-C row column)
                 (aref matrix-C row column)
                 0)))
      
      (dotimes (r rows)
        (dotimes (c columns)
          
          (setf (aref matrix-C r c)
                (if (equal? (nth r seq-S) (nth c seq-T))
                    (1+ (at (1- r) (1- c)))
                    (max (at r (1- c))
                         (at (1- r) c))))))
      
      matrix-C)))


; Test.
(let ((seq-S (split "xyzxzyxxyz"))
      (seq-T (split "zyzzzxzxyy"))
      (matrix-C #2A((0 0 0 0 0 1 1 1 1 1)
                    (0 1 1 1 1 1 1 1 2 2)
                    (1 1 2 2 2 2 2 2 2 2)
                    (1 1 2 2 2 3 3 3 3 3)
                    (1 1 2 3 3 3 4 4 4 4)
                    (1 2 2 3 3 3 4 4 5 5)
                    (1 2 2 3 3 4 4 5 5 5)
                    (1 2 2 3 3 4 4 5 5 5)
                    (1 2 2 3 3 4 4 5 6 6)
                    (1 2 3 3 4 4 5 5 6 6))))
  
  (assert (equal? matrix-C (compute-matrix seq-S seq-T))))


; Start.
(let* ((seq-S (progn (princ "Sequence S: ")
                     (split (trim (read-line)))))
       (seq-T (progn (princ "Sequence T: ")
                     (split (trim (read-line)))))
       (matrix-C (compute-matrix seq-S seq-T)))
  
  (format t "~%Matrix C:~%")

  (princ " ")
  (dolist (char seq-T) (format t " ~A" char))
  (new-line)
  
  (dotimes (r (length seq-S))
    (princ (nth r seq-S))
    (dotimes (c (length seq-T)) (format t " ~A" (aref matrix-C r c)))
    (new-line)))
