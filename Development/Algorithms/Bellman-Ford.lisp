(load "Lispy.lisp")


(define-class Edge ()
  (source destiny weight))


(defconstant +infinity most-positive-fixnum)


(defun compute-distances (source vertices edges &optional (verbose? false))
  (let ((distances (make-array vertices :initial-element +infinity)))
    
    (flet ((relax (edge)
             (let ((from (aref distances (source-of edge)))
                   (to (aref distances (destiny-of edge))))
               
               (unless (= from +infinity)
                 (incf from (weight-of edge))
                 
                 (when (> to from)
                   (setf (aref distances (destiny-of edge)) from))))))
      
      (setf (aref distances source) 0)
      
      (dotimes (iter (1- vertices))
        (map 'list #'relax edges)
        
        (when verbose?
          (format t "~%(~A)~%" (1+ iter))
          
          (princ "Vertices:")
          (dotimes (i vertices) (format t " ~A" i))
          (new-line)
          
          (princ "Distance:")
          (dotimes (i vertices)
            (let ((d (aref distances i)))
              (format t " ~A" (if (= d +infinity) #\Infinity d))))
          (new-line)))
      
      (dolist (edge edges)
        (let ((from (aref distances (source-of edge)))
              (to (aref distances (destiny-of edge)))
              (weight (weight-of edge)))
          
          (when (< (+ from weight) to)
            (return-from compute-distances nil))))
      
      distances)))


; Test.
(let ((source 4)
      (vertices 5)
      (edges (mapcar (bind #'apply #'Edge)
                     '((:source 0 :destiny 1 :weight  5)
                       (:source 0 :destiny 2 :weight  8)
                       (:source 0 :destiny 3 :weight -4)
                       (:source 1 :destiny 0 :weight -2)
                       (:source 2 :destiny 1 :weight -3)
                       (:source 2 :destiny 3 :weight  9)
                       (:source 3 :destiny 1 :weight  7)
                       (:source 3 :destiny 4 :weight  2)
                       (:source 4 :destiny 0 :weight  6)
                       (:source 4 :destiny 2 :weight  7))))
      (distances #1A(2 4 7 -2 0)))
  
  (assert (equal? distances (compute-distances source vertices edges))))


; Start.
(let ((source (progn (princ "Source: ") (read)))
      (edges ())
      (max 0))
  
  (write-line "Edges, format \"(source destiny weight)\", \"s\" to stop:")
  (loop
    (princ "> ")
    
    (let ((edge (read)))
      (if (equal? edge 's)
          (return)
          (push (setf edge (apply #'make-edge edge)) edges))
      
      (when (> (source-of edge) max)
        (setf max (source-of edge)))
      (when (> (destiny-of edge) max)
        (setf max (destiny-of edge)))))
  
  (let ((distances (compute-distances source (1+ max) (reverse edges) true)))
    (when (null distances)
      (write-line "Negative edge weight cycle detected."))))
