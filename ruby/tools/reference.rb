class Reference
  # Make sure that the Reference class has no normal methods anymore.
  methods.each do |method|
    begin
      undef_method(method)
    rescue NameError
    end if !method[/^__.+__$/]
  end

  def initialize(content)
    @content=content
  end

  # Since we have no normal methods anymore, all of them will be passed to the content via send()
  def method_missing(method,*args)
    @content.send(method,*args)
  end

  # Special methods to access the content itself
  def []
    @content    
  end
  def []=(arg)
    @content=arg
  end
end

class Object
  # Since the object_id method is removed for a Reference and the object_id of the content will be used instead, a Reference object can be tested as such (__id__ is not overwritten)
  def is_reference?
    object_id!=__id__
  end
end
