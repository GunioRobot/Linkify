(define-class Year ()
  (start terms))


(defparameter *years*
  (list
    (Year :start 2004 :terms
          '(("AL - Algebra Linear"
             "AMI - Analise Matematica I"
             "FP - Fundamentos de Programacao"
             "SDig - Sistemas Digitais"
             "TC - Teoria da Computacao")
            ("AC - Arquitectura de Computadores"
             "AED - Algoritmos e Estruturas de Dados"
             "AMII - Analise Matematica II"
             "FI - Fisica I"
             "MD - Matematica Discreta")))
    
    (Year :start 2005 :terms
          '(("AMIII - Analise Matematica III"
             "FG - Fundamentos de Gestao"
             "FII - Fisica II"
             "PO - Programacao com Objectos"
             "SO - Sistemas Operativos")
            ("AMIV - Analise Matematica IV"
             "C - Compiladores"
             "CG - Computacao Grafica"
             "IA - Inteligencia Artificial"
             "PE - Probabilidades e Estatistica")))
    
    (Year :start 2006 :terms
          '(("BD - Bases de Dados"
             "IPM - Interface Pessoa Maquina"
             "PPI - Portfolio Pessoal I"
             "RCI, RC - Redes de Computadores I, Redes de Computadores"
             "SS - Sistemas e Sinais")
            ("ASA - Analise e Sintese de Algoritmos"
             "ES - Engenharia de Software"
             "PPII - Portfolio Pessoal II"
             "RC - Representacao do Conhecimento"
             "SD - Sistemas Distribuidos")))
    
    (Year :start 2007 :terms
          '(("ASof - Arquitecturas de Software"
             "AVT - Animacao e Visualizacao Tridimensional"
             "GPI - Gestao de Projectos Informaticos"
             "PPIII - Portfolio Pessoal III"
             "QS - Qualidade de Software")
            ("AASMA - Agentes Autonomos e Sistemas Multi-Agente"
             "PADI - Plataformas para Aplicacoes Distribuidas na Internet"
             "PAva - Programacao Avancada"
             "PCM - Producao de Conteudos Multimedia"
             "PPIV - Portfolio Pessoal IV")))
    
    (Year :start 2008 :terms
          '(("CCom - Complementos de Compiladores"
             "LN - Lingua Natural"
             "PLP - Pragmatica das Linguagens de Programacao")
            ()))))


(XML
  (:p "Here you can find the archive (in Portuguese) of all the courses I had so far.")
  
  (let ((base-URL "http://dl.getdropbox.com/u/1055317/Ensino Superior/"))
    (dotimes (i (length *years*))
    
      (XML
        (:table (@ (:class "Courses"))
          (let ((year (nth i *years*))
                (year-part (format nil "Ano ~A/" (+ i 1))))
            
            (XML
              (:thead
                (:tr
                  (:th (@ (:colspan (? length (terms-of year))))
                       (? format nil "Year ~A-~A" (start-of year)
                                                  (+ 1 (start-of year)))))))
            
            (XML
              (:tbody
                (:tr (dotimes (j (length (terms-of year)))
                       (XML (:td (? format nil "Term ~A" (+ j 1))))))
                
                (:tr (dotimes (j (length (terms-of year)))
                       (let ((term (nth j (terms-of year)))
                             (term-part (format nil "Semestre ~A/" (+ j 1))))
                         
                         (unless (empty? term)
                           (XML
                             (:td
                               (:ul
                                 (dolist (course term)
                                   (let ((file (format nil "~A.7z" course)))
                                     
                                     (XML
                                       (:li
                                         (:a (@ (:href (? concatenate 'string
                                                          base-URL
                                                          year-part
                                                          term-part
                                                          file)))
                                             course))))))))))))))))))))
