(define-class Course ()
  (name path))

(define-class Year ()
  (span terms))


(defmethod URL-of ((course Course))
  (concatenate 'string "http://marcio.comoj.com/ist/" (path-of course)))


(defparameter *y1-t1* (list (Course :name "AL - Álgebra Linear"
                                    :path "ano1/sem1/AL.7z")
                            (Course :name "AMI - Análise Matemática I"
                                    :path "ano1/sem1/AMI.7z")
                            (Course :name "FP - Fundamentos de Programação"
                                    :path "ano1/sem1/FP.7z")
                            (Course :name "SDig - Sistemas Digitais"
                                    :path "ano1/sem1/SDig.7z")
                            (Course :name "TC - Teoria da Computação"
                                    :path "ano1/sem1/TC.7z")))

(defparameter *y1-t2* (list (Course :name "AC - Arquitectura de Computadores"
                                    :path "ano1/sem2/AC.7z")
                            (Course :name "AED - Algoritmos e Estruturas de Dados"
                                    :path "ano1/sem2/AED.7z")
                            (Course :name "AMII - Análise Matemática II"
                                    :path "ano1/sem2/AMII.7z")
                            (Course :name "FI - Física I"
                                    :path "ano1/sem2/FI.7z")
                            (Course :name "MD - Matemática Discreta"
                                    :path "ano1/sem2/MD.7z")))

(defparameter *y2-t1* (list (Course :name "AMIII - Análise Matemática III"
                                    :path "ano2/sem1/AMIII.7z")
                            (Course :name "FG - Fundamentos de Gestão"
                                    :path "ano2/sem1/FG.7z")
                            (Course :name "FII - Física II"
                                    :path "ano2/sem1/FII.7z")
                            (Course :name "PO - Programação com Objectos"
                                    :path "ano2/sem1/PO.7z")
                            (Course :name "SO - Sistemas Operativos"
                                    :path "ano2/sem1/SO.7z")))

(defparameter *y2-t2* (list (Course :name "AMIV - Análise Matemática IV"
                                    :path "ano2/sem2/AMIV.7z")
                            (Course :name "C - Compiladores"
                                    :path "ano2/sem2/C.7z")
                            (Course :name "CG - Computação Gráfica"
                                    :path "ano2/sem2/CG.7z")
                            (Course :name "IA - Inteligência Artificial"
                                    :path "ano2/sem2/IA.7z")
                            (Course :name "PE - Probabilidades e Estatística"
                                    :path "ano2/sem2/PE.7z")))

(defparameter *y3-t1* (list (Course :name "BD - Bases de Dados"
                                    :path "ano3/sem1/BD.7z")
                            (Course :name "IPM - Interface Pessoa Máquina"
                                    :path "ano3/sem1/IPM.7z")
                            (Course :name "PPI - Portfólio Pessoal I"
                                    :path "ano3/sem1/PPI.7z")
                            (Course :name "RCI, RC - Redes de Computadores I, Redes de Computadores"
                                    :path "ano3/sem1/RCI-RC.7z")
                            (Course :name "SS - Sistemas e Sinais"
                                    :path "ano3/sem1/SS.7z")))

(defparameter *y3-t2* (list (Course :name "ASA - Análise e Síntese de Algoritmos"
                                    :path "ano3/sem2/ASA.7z")
                            (Course :name "ES - Engenharia de Software"
                                    :path "ano3/sem2/ES.7z")
                            (Course :name "PPII - Portfólio Pessoal II"
                                    :path "ano3/sem2/PPII.7z")
                            (Course :name "RC - Representação do Conhecimento"
                                    :path "ano3/sem2/RC.7z")
                            (Course :name "SD - Sistemas Distribuídos"
                                    :path "ano3/sem2/SD.7z")))

(defparameter *y4-t1* (list (Course :name "ASof - Arquitecturas de Software"
                                    :path "ano4/sem1/ASof.7z")
                            (Course :name "AVT - Animação e Visualização Tridimensional"
                                    :path "ano4/sem1/AVT.7z")
                            (Course :name "GPI - Gestão de Projectos Informáticos"
                                    :path "ano4/sem1/GPI.7z")
                            (Course :name "PPIII - Portfólio Pessoal III"
                                    :path "ano4/sem1/PPIII.7z")
                            (Course :name "QS - Qualidade de Software"
                                    :path "ano4/sem1/QS.7z")))

(defparameter *y4-t2* (list (Course :name "AASMA - Agentes Autónomos e Sistemas Multi-Agente"
                                    :path "ano4/sem2/AASMA.7z")
                            (Course :name "PADI - Plataformas para Aplicações Distribuídas na Internet"
                                    :path "ano4/sem2/PADI.7z")
                            (Course :name "PAva - Programação Avançada"
                                    :path "ano4/sem2/PAva.7z")
                            (Course :name "PCM - Produção de Conteúdos Multimédia"
                                    :path "ano4/sem2/PCM.7z")
                            (Course :name "PPIV - Portfólio Pessoal IV"
                                    :path "ano4/sem2/PPIV.7z")))

(defparameter *y5-t1* (list (Course :name "CCom - Complementos de Compiladores"
                                    :path "ano5/sem1/CCom.7z")
                            (Course :name "LN - Língua Natural"
                                    :path "ano5/sem1/LN.7z")
                            (Course :name "PLP - Pragmática das Linguagens de Programação"
                                    :path "ano5/sem1/PLP.7z")))

(defparameter *y5-t2* (list))


(defparameter *years* (list (Year :span "2004-2005" :terms (list *y1-t1* *y1-t2*))
                            (Year :span "2005-2006" :terms (list *y2-t1* *y2-t2*))
                            (Year :span "2006-2007" :terms (list *y3-t1* *y3-t2*))
                            (Year :span "2007-2008" :terms (list *y4-t1* *y4-t2*))
                            (Year :span "2008-2009" :terms (list *y5-t1* *y5-t2*))))


(XML
  (:p "Here you can find the archive (in Portuguese) of all the courses I had so far.")
  (:p "When downloading a file, if it appears to stop for no reason just resume it or use a download manager.")
  
  (dolist (year *years*)
    (XML
      (:table
        (@ (:class "Courses"))
        
        (:thead (:tr (:th (@ (:colspan (? length (terms-of year))))
                          (? format nil "Year ~A" (span-of year)))))
        
        (:tbody (:tr (dotimes (i (length (terms-of year)))
                       (XML (:td (? format nil "Term ~A" (+ i 1))))))
                
                (:tr (dolist (term (terms-of year))
                       (XML (:td (:ul (dolist (course term)
                                        (XML (:li (:a (@ (:href (? URL-of course)))
                                                      (? name-of course)))))))))))))))
