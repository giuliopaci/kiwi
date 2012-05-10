#!/usr/bin/env ruby

require 'rubygems'
require 'ffi'

class Node < FFI::Struct
  layout :name, :pointer,
         :content, :pointer,
         :level, :uint64,
         :next, :pointer # This is a pointer to a Node... Don't know how to do that yet...
end

module BString
  extend FFI::Library
  ffi_lib File.join(File.dirname(__FILE__), '..', 'libkiwi.so')
  # char * bstr2cstr (const_bstring s, char z)
  attach_function :bstr2cstr, [:pointer, :char], :string
  # int bcstrfree (char * s)
  attach_function :bcstrfree, [:string], :int
end

module YAPWTP
  extend FFI::Library
  ffi_lib File.join(File.dirname(__FILE__), '..', 'libkiwi.so')
  # void init(void)
  attach_function :kw_init, [], :pointer
  # void cleanup(void)
  attach_function :kw_cleanup, [:pointer], :void

  # void parse(bstring inputbuffer, bstring outbuffer);
  attach_function :kw_parse, [:pointer], :void

  # void stdin_get_contents(bstring buffer)
  attach_function :stdin_get_contents, [:pointer], :void
  # void file_get_contents(bstring buffer, char *filename)
  attach_function :file_get_contents, [:pointer, :string], :void
  # void str_get_contents(const char *str)
  attach_function :str_get_contents, [:pointer, :string], :void

  # bstring get_input_buffer(void)
  attach_function :kw_get_input_buffer, [:pointer], :pointer
  # char * get_output_buffer_cstr(void)
  attach_function :kw_get_output_buffer_cstr, [:pointer], :string

  # void set_base_url(char *str)
  attach_function :kw_set_base_url, [:pointer, :string], :void
  # void set_image_base_url(char *str)
  attach_function :kw_set_image_base_url, [:pointer, :string], :void

  # void reset_template_iter(void)
  attach_function :reset_template_iter, [:pointer], :void
  # struct node *get_next_template(void)
  attach_function :get_next_template, [:pointer], :pointer
  # int get_template_count(void)
  attach_function :get_template_count, [:pointer], :int
end

class WikiParser
  include YAPWTP
  extend YAPWTP

  def initialize
    setup
  end

  def setup
    @kw = kw_init
    @dirty = false
    @output = nil
    @templates = nil
  end

  def reset
    kw_cleanup @kw
    setup
  end

  def template_count
    return get_template_count @kw
  end

  private
  def next_template
    return {} if !@dirty
    t = get_next_template @kw
    return nil if t.null?
    template = Node.new(t)
    name = BString.bstr2cstr(template[:name], 20)
    content = BString.bstr2cstr(template[:content], 20)
    # We don't need to free our strings created with bstr2cstr, because FFI
    # seems to recognize them as char*'s and frees them when they fall out of
    # scope.
    return { :name => name,
             :content => content, 
             :hash => template[:level] + 0,
             :replace_tag => "__#{name}_#{template[:level] + 0}__"
           }
  end

  public
  def parsed_text
    if @dirty
      @output ||= String.new(kw_get_output_buffer_cstr @kw)
    else
      nil
    end
  end

  def html_from_string source
    reset if @dirty
    # The parser wants a newline or EOF at the end of the file...
    if source[-1] != "\n"
      source << "\n"
    end
    str_get_contents @kw, source
    kw_parse @kw
    @dirty = true
    return parsed_text
  end

  def html_from_file file
    reset if @dirty
    if !File.exist? file
      raise IOError("Can't open #{file}")
    end
    file_get_contents kw_get_input_buffer(@kw), file
    kw_parse @kw
    @dirty = true
    return parsed_text
  end

  def each_template
    return nil if !@dirty

    reset_template_iter @kw
    while template = next_template
      yield template
    end
  end

  def templates
    return @templates if @templates

    @templates = []
    each_template { |t| @templates << t }
    @templates
  end

  def base_url= url
    kw_set_base_url @kw, url
  end

  def image_base_url= url
    kw_set_image_base_url @kw, url
  end
end

