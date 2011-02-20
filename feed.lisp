(let ((*load-verbose* nil))
  (require :cl-ppcre)
  (require :metabang-bind)
  (require :trivial-http)
  (require :xmls))

(import 'metabang-bind:bind)

(defun ign-daily-fix-item? (item)
  (let ((title (find-if (lambda (element) (equalp (car element) "title"))
                        (cddr item))))
    (cl-ppcre:scan "(?ix) ign \\s+ daily \\s+ fix"
                   (nth 2 title))))

(bind (((status headers feed-data)
        (trivial-http:http-get "http://feeds.ign.com/ignfeeds/podcasts/games/"))
       (feed (xmls:parse feed-data))
       (channel (nth 2 feed)))
  (delete-if
    (lambda (item)
      (and (equalp (car item) "item")
           (not (ign-daily-fix-item? item))))
    (cddr channel))
  (xmls:write-xml feed *standard-output*))
