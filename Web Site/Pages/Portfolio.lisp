(XML
  (:p "Check out "
      (:a (@ (:href "http://subversion.assembla.com/svn/marciof"))
          "my public SVN repository")
      " which also has a "
      (:a (@ (:href "http://code.assembla.com/marciof/subversion/nodes"))
          "web interface")
      " available.")
  
  (:p "It contains this Web site's code and other minor projects, such as:")
  
  (:ul
    (:li "Simple "
         (:a (@ (:href "http://code.assembla.com/marciof/subversion/nodes/Attic/Garbage Collector"))
             "Garbage Collector")
         " for C++.")
    (:li "Dynamic "
         (:a (@ (:href "http://code.assembla.com/marciof/subversion/nodes/Attic/Java Object Inspector"))
             "object inspector")
         " for Java.")
    (:li "RSS "
         (:a (@ (:href "http://code.assembla.com/marciof/subversion/nodes/Attic/RSS Generator"))
             "feed generator")
         ", also available "
         (:a (@ (:href "http://web.ist.utl.pt/marcio.faustino/rss/")) "online")
         ".")
    (:li (:a (@ (:href "http://code.assembla.com/marciof/subversion/nodes/Attic/publish.pl"))
             "Wrapper script")
         " to simplify the publishing process of DocBook to HTML."))
  
  (:h3 "Projects")
  
  (:dl
    (:dt (:a (@ (:href "http://eon.origo.ethz.ch/")) "Eon"))
    (:dd "Kernel experiment to learn more about the design and implementation of Operating Systems.")
    
    (:dt (:a (@ (:href "http://janus.sourceforge.net/")) "Janus"))
    (:dd "Software library to ease development in the C programming language.")))
