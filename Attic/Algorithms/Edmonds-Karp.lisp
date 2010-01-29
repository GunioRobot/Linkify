(load "Lispy.lisp")


(define-class Edge ()
  (source destiny capacity))


(defconstant +infinity most-positive-fixnum)


(defun compute-max-flow (source sink capacities &optional (verbose? false))
  (let* ((size (array-dimension capacities 0))
         (dimensions (array-dimensions capacities))
         
         (flow (make-array dimensions :initial-element 0))
         (residual-capacities (make-array dimensions :initial-element 0))
         
         (parent (make-array size :initial-element 0))
         (minimum-capacity (make-array size :initial-element 0))
         
         (max-flow ())
         (iteration 0))
    
    (flet ((exists-path? ()
             (let ((color (make-array size :initial-element 'white))
                   (queue (make-array size))
                   (first 0)
                   (last 0))
               
               (dotimes (i size) (setf (aref minimum-capacity i) +infinity))
               (setf (aref queue last) source)
               (setf (aref color source) 'gray)
               (incf last)
               
               (until (= first last)
                 (let ((v (aref queue first)))
                   (incf first)
                   
                   (dotimes (u size)
                     (when (and (equal? (aref color u) 'white)
                                (> (aref residual-capacities v u) 0))
                       
                       (setf (aref minimum-capacity u)
                             (min (aref minimum-capacity v)
                                  (aref residual-capacities v u)))
                       
                       (setf (aref parent u) v)
                       (setf (aref color u) 'gray)
                       
                       (if (= u sink) (return-from exists-path? true))
                       
                       (setf (aref queue last) u)
                       (incf last)))))
               
               false)))
      
      (dotimes (from size)
        (dotimes (to size)
          (setf (aref residual-capacities from to)
                (aref capacities from to))))
      
      (while (exists-path?)
        (push (aref minimum-capacity sink) max-flow)
        (incf iteration)
        
        (let ((v sink))
          (until (= v source)
            (let ((u (aref parent v))
                  (cap (aref minimum-capacity sink)))
              
              (incf (aref flow u v) cap)
              (decf (aref residual-capacities u v) cap)
              
              (setf v u))))
        
        (when verbose?
          (format t "Iteration: ~A~%" iteration)
          
          (write-line "Residual capacity matrix:")
          (dotimes (i size)
            (princ " ")
            (dotimes (j size)
              (let ((c (aref residual-capacities i j)))
                (format t " ~A" (if (= c 0) '- c))))
            (new-line))
          
          (write-line "Flow matrix:")
          (dotimes (i size)
            (princ " ")
            (dotimes (j size)
              (let ((f (aref flow i j)))
                (format t " ~A" (if (= f 0) '- f))))
            (new-line))
          
          (new-line)))
      
      (reverse max-flow))))


; Test.
(let ((source 0)
      (sink 5)
      (capacities #2A((0 5 0 10 0 0)
                      (0 0 9  0 0 0)
                      (0 0 0  0 6 4)
                      (0 3 0  0 2 0)
                      (0 0 0  0 0 9)
                      (0 0 0  0 0 0))))
  
  (assert (equal? (compute-max-flow source sink capacities)
                  '(4 2 1 3))))


; Start.
(let* ((source (progn (princ "Source: ") (read)))
       (sink (progn (princ "Sink: ") (read)))
       (edges ())
       (max 0))
  
  (write-line "Edges, format \"(source destiny capacity)\", \"s\" to stop:")
  (loop
    (princ "> ")
    
    (let ((edge (read)))
      (if (equal? edge 's)
          (return)
          (let ((s (first edge))
                (d (second edge))
                (c (third edge)))
            (push (setf edge (Edge :source s :destiny d :capacity c)) edges)))
      
      (when (> (source-of edge) max)
        (setf max (source-of edge)))
      (when (> (destiny-of edge) max)
        (setf max (destiny-of edge)))))
  
  (let* ((size (1+ max))
         (capacities (make-array (list size size) :initial-element 0)))
    
    (dolist (e edges)
      (setf (aref capacities (source-of e) (destiny-of e))
            (capacity-of e)))
    
    (let ((max-flow (compute-max-flow source sink capacities true)))
      
      (format t "Maximum flow: ~A = ~A~%"
                (join max-flow " + ")
                (reduce #'+ max-flow)))))
