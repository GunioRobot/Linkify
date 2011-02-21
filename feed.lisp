(let ((*load-verbose* nil))
  (require :cl-ppcre)
  (require :metabang-bind)
  (require :trivial-http)
  (require :xmls))

(import 'metabang-bind:bind)


(defun filter-feed-items (url predicate)
  (bind (((status headers data) (trivial-http:http-get url))
         (feed (xmls:parse data))
         (channel (nth 2 feed)))
    
    (delete-if
      (lambda (item)
        (and (equalp (car item) "item")
             (not (funcall predicate item))))
      (cddr channel))
    
    (xmls:write-xml feed *standard-output*)))


(defun hd-trailer-item? (item)
  (let ((title (find-if (lambda (element) (equalp (car element) "title"))
                        (cddr item))))
    (cl-ppcre:scan "(?ix) \\( [^(]* (teaser | trailer) [^)]* \\)"
                   (nth 2 title))))


(defun ign-daily-fix-item? (item)
  (let ((title (find-if (lambda (element) (equalp (car element) "title"))
                        (cddr item))))
    (cl-ppcre:scan "(?ix) ign \\s+ daily \\s+ fix"
                   (nth 2 title))))


(filter-feed-items
  "http://feeds.hd-trailers.net/hd-trailers/blog"
  #'hd-trailer-item?)


;; (filter-feed-items
;;   "http://feeds.ign.com/ignfeeds/podcasts/games/"
;;   #'ign-daily-fix-item?)
