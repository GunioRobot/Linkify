(load "Lispy.lisp")


(define-class Web-Page ()
  (title))

(define-class Web-Site ()
  (author footer icon language logos menu rss scripts style title widget))


(defmethod compiled-file-of ((page Web-Page))
  (concatenate 'string (title-of page) ".html"))


(defmethod file-of ((page Web-Page))
  (concatenate 'string "Pages/" (title-of page) ".lisp"))


(defmethod id-of ((page Web-Page))
  (let* ((id (string-downcase (trim (title-of page))))
         (index (position-if (lambda (char) (find char *white-space*)) id)))
    
    (if index
        (subseq id 0 index)
        id)))


(defmethod parse-arguments (arguments)
  (mapcar (lambda (argument)
            (if (equal? 0 (position #\: argument))
                (read-from-string argument)
                argument))
          arguments))


(defmethod create ((site Web-Site) &key compiling? select)
  (let* ((pages (mapcar (bind #'Web-Page :title) (menu-of site)))
         (current-page (first pages)))
    
    (when (equal? select :all)
      (dolist (page pages)
        (with-open-file (stream (compiled-file-of page) :direction :output)
          (let ((*standard-output* stream))
            (create site :compiling? true :select (id-of page)))))
      
      (with-open-file (stream "index.html" :direction :output)
        (let ((*standard-output* stream))
          (format t "<?php include ~S; exit; ?>~%" (compiled-file-of current-page))
          (create site :compiling? true :select (id-of current-page))))
      
      (return-from create))
    
    (when select
      (let ((page (find select pages :key #'id-of :test #'equal?)))
        (when page (setf current-page page))))
    
    (XML
      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
      #\Newline
      "  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
      #\Newline
      
      (:html
        (@ (:xmlns "http://www.w3.org/1999/xhtml")
           (:lang (? language-of site)))
        
        (:head
           (:title (? concatenate 'string (title-of site) " - " (title-of current-page)))
           
           (:meta (@ (:http-equiv "Content-Type")
                     (:content "text/html; charset=UTF-8")))
           (:meta (@ (:name "Generator")
                     (:content "Lisp")))
           
           (when (author-of site)
             (XML (:meta (@ (:name "Author")
                            (:content (? author-of site))))))
           
           (:link (@ (:rel "StyleSheet")
                     (:type "text/css")
                     (:media (? style-of site))
                     (:href (? concatenate 'string (style-of site) ".css"))))
           
           (when (rss-of site)
             (let ((title (car (rss-of site)))
                   (link (cdr (rss-of site))))
               (XML (:link (@ (:rel "Alternate")
                              (:type "application/rss+xml")
                              (:title title)
                              (:href link))))))
           
           (when (widget-of site)
             (let ((title (car (widget-of site)))
                   (link (cdr (widget-of site))))
               (XML (:link (@ (:rel "Alternate")
                              (:type "application/x-opera-widgets")
                              (:title title)
                              (:href link))))))
           
           (when (icon-of site)
             (XML (:link (@ (:rel "icon")
                            (:type "image/ico")
                            (:href (? icon-of site))))
                  (:link (@ (:rel "shortcut icon")
                            (:type "image/ico")
                            (:href (? icon-of site))))))
           
           (dolist (path (scripts-of site))
             (XML (:script (@ (:type "text/javascript") (:src path))))))
        
        (:body
          (:div (@ (:class "Header"))
                (:h1 (@ (:class "Title"))
                     (? title-of site)))
          
          (:div (@ (:class "Menu"))
                (:ul (@ (:class "Menu-List"))
                     (dolist (page pages)
                       
                       (let* ((selected (if (equal? page current-page) " Selected" ""))
                              (classes (concatenate 'string "Menu-List-Item" selected))
                              (link (if compiling?
                                        (compiled-file-of page)
                                        (concatenate 'string "?page=" (id-of page)))))
                         
                         (XML (:li (@ (:class classes))
                                   (:a (@ (:href link))
                                       (? title-of page))))))))
          
          (:div (@ (:class "Document"))
                (load (file-of current-page)))
          
          (:div (@ (:class "Footer"))
                
                (dolist (line (footer-of site))
                  (XML (:p (@ (:class (? car line)))
                           (? cdr line))))
                
                (dolist (logo (logos-of site))
                  (let ((image (cdr (assoc :image logo)))
                        (link (cdr (assoc :link logo)))
                        (text (cdr (assoc :text logo))))
                    
                    (XML (:a (@ (:class "Logo")
                                (:href link))
                             (:img (@ (:src image)
                                      (:alt text)))))))))))))


(defun Settings (&rest options)
  (defparameter *web-site* (apply #'Web-Site options))
  (apply #'create *web-site* (parse-arguments ext:*ARGS*))
  (makunbound '*web-site*))


(load "Web.cfg")
