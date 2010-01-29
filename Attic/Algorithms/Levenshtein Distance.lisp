(load "Lispy.lisp")


(defparameter *insert-cost* 2)
(defparameter *replace-cost* 3)
(defparameter *remove-cost* 1)


(defun simple-cost (operation &optional out in)
  (case operation
    ('insert *insert-cost*)
    ('replace (if (equal? out in) 0 *replace-cost*))
    ('remove *remove-cost*)))


(defun compute-distance (output input cost &optional (verbose? false))
  (let* ((output-size (length output))
         (input-size (length input))
         (distance (make-array (list (1+ output-size) (1+ input-size)))))
    
    (setf (aref distance 0 0) 0)
    
    (dotimes (column output-size)
      (let ((c (char output column)))
        
        (setf (aref distance (1+ column) 0)
              (+ (aref distance column 0)
                 (funcall cost 'insert c nil)))))
    
    (dotimes (row input-size)
      (let ((r (char input row)))
        
        (setf (aref distance 0 (1+ row))
              (+ (aref distance 0 row)
                 (funcall cost 'remove nil r)))))
    
    (dotimes (row input-size)
      (let ((r (char input row)))
        
        (dotimes (column output-size)
          (let ((c (char output column)))
            
            (setf (aref distance (1+ column) (1+ row))
                  (min (+ (aref distance column (1+ row))
                          (funcall cost 'insert c nil))
                       (+ (aref distance column row)
                          (funcall cost 'replace c r))
                       (+ (aref distance (1+ column) row)
                          (funcall cost 'remove nil r))))))))
    
    (when verbose?
      (format t "~%Matrix:~%")
      
      (dotimes (row (1+ input-size))
        (princ "  ")
        
        (if (< row input-size)
            (format t "~A" (char input (- input-size row 1)))
            (princ "#"))
        
        (dotimes (column (1+ output-size))
          (format t " ~A" (aref distance column (- input-size row))))
        (new-line))
      
      (format t "    # ~A~%" (join (split output) " ")))
    
    (values (aref distance output-size input-size) distance)))


; Test.
(let ((input "fazenda")
      (output "azedo")
      (distances #2A(( 0  1 2 3 4 5 6 7)
                     ( 2  3 1 2 3 4 5 6)
                     ( 4  5 3 1 2 3 4 5)
                     ( 6  7 5 3 1 2 3 4)
                     ( 8  9 7 5 3 4 2 3)
                     (10 11 9 7 5 6 4 5))))
  
  (multiple-value-bind (value matrix)
    (compute-distance output input #'simple-cost)
    
    (assert (= value 5))
    (assert (equal? matrix distances))))


(let* ((input (progn (princ "Input: ") (trim (read-line))))
       (output (progn (princ "Output: ") (trim (read-line))))
       (costs (progn (princ "Costs, format \"(i s r)\": ") (trim (read-line)))))
  
  (unless (empty? costs)
    (setf costs (read-from-string costs))
    (setf *insert-cost* (first costs)
          *replace-cost* (second costs)
          *remove-cost* (third costs)))
  
  (format t "~%Distance: ~A~%"
          (compute-distance output input #'simple-cost true)))
