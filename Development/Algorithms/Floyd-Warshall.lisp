(load "Lispy.lisp")


(defconstant +infinity most-positive-fixnum)


(defun compute-distances (weights &optional (verbose? false))
  (let ((vertices (array-dimension weights 0))
        (distances weights))
    
    (dotimes (k vertices distances)
      (let ((next (make-array (array-dimensions weights))))
        
        (dotimes (i vertices)
          (dotimes (j vertices)
            (let ((d (aref distances i j)))
              
              (setf (aref next i j)
                    (if (or (= (aref distances i k) +infinity)
                            (= (aref distances k j) +infinity))
                        d
                        (min d (+ (aref distances i k)
                                  (aref distances k j))))))))
        
        (setf distances next)
        
        (when verbose?
          (format t "~%Matrix D^~A:~%" (1+ k))
          
          (dotimes (i vertices)
            (princ " ")
            (dotimes (j vertices)
              (let ((d (aref distances i j)))
                (format t " ~A" (if (= d +infinity) #\Infinity d))))
            (new-line)))))))


; Test.
(let* ((i +infinity)
       (weights (make-array '(5 5) :initial-contents `(( 0  3  8 ,i -4)
                                                       (,i  0 ,i  1  7)
                                                       (,i  4  0 ,i ,i)
                                                       ( 2 ,i -5  0 ,i)
                                                       (,i ,i ,i  6  0))))
       (distances #2A((0  1 -3  2 -4)
                      (3  0 -4  1 -1)
                      (7  4  0  5  3)
                      (2 -1 -5  0 -2)
                      (8  5  1  6  0))))
  
  (assert (equal? distances (compute-distances weights))))


; Start.
(write-line "Weights matrix, list format, \"i\" for infinity:")

(let* ((weights (subst +infinity 'i (read)))
       (dimensions (list (length weights) (length weights)))
       (weights-matrix (make-array dimensions :initial-contents weights)))
  
  (compute-distances weights-matrix true))
