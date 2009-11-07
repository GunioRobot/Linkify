#-CLISP (error "Only CLISP is supported.")


(defmacro macro (name &environment env)
  "Converts a macro to a function, e.g. (funcall (macro and) 1 2) --> 2"
  (let ((local-env (gensym))
        (macro-env (gensym)))
    
    `(let ((,local-env (ext:the-environment))
           (,macro-env ,env))
       
       (lambda (&rest arguments)
         (ext:eval-env (funcall (macro-function ',name ,macro-env)
                                (cons ',name arguments)
                                ,macro-env)
                       ,local-env)))))


; Test.
(let ((a 1) (b 2))
  (macrolet ((-and (x y) `(and ,x ,y)))
    (assert (= (apply (macro -and) (list a b)) 2))
    (assert (= (funcall (macro incf) 'a 2) 3))))
