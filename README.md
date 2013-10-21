Kiwi
======

Kiwi is a C implementation of the vast majority of MediaWiki's
wikitext syntax.  It currently takes input on stdin and presents
output on stdout, and there is a Ruby FFI module for direct library
use as well.  The major advantages of this implementation are
intended to be speed and memory footprint.

At the moment a 100 line wikitext file with fairly complex markup
can be parsed in 5-6ms on a one year old Apple MacBook Pro.

See it Live
-----------

Kiwi is running on the web at [drasticcode.com](http://kiwi.drasticcode.com).  Check it out
there for a better list of capabilities.

Simplest Ruby Example with Templates
------------------------------------
    require "yapwtp"
    
    parser = WikiParser.new
    
    # Open the requested file, parse, capture text and templates
    wikitext = parser.html_from_file("cnn.com.wt")
    templates = parser.templates
    
    templates.each do |template|
      template_file = File.join("templates", template[:name]) 
      if File.exist? template_file
        wikitext.gsub! /#{template[:replace_tag]}/, parser.html_from_file(template_file)
      end
    end
    puts wikitext

pegleg documentation
====================

see http://piumarta.com/software/peg/peg.1.html for help with the syntax
