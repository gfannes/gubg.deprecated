require("gubg/utils")
require("gubg/token")

$dumpSize = 100

module Loggable
	def log(*args)
		puts(*args) if false
	end
end

class TranslationUnit
	include Loggable
	extend Loggable
    attr_reader(:file)
    def initialize(na = {file: nil, content: nil})
        @file = na[:file]
        @content = (na[:content] || String.loadBinary(@file)).freeze
	begin
		@declarations = TranslationUnit.parseDeclarations_(Token.new(@content))
	rescue => exc
		log("Parsing failure: #{exc}\n#{exc.backtrace*"\n"}")
		log("Problem occured in #{@file}")
		exit(-1)
	end
        parsed = TranslationUnit.generate_(@declarations)
        if @content != parsed
            fnOrig, fnParsed = tempFile("orig.cpp"), tempFile("parsed.cpp")
            @content.export(fnOrig)
            parsed.export(fnParsed)
            system("kdiff3 #{fnOrig} #{fnParsed}")
            raise("Parsed result is different from original")
        end
    end
    def export(filename)
        TranslationUnit.generate_(@declarations).export(filename)
    end
    def each(meta, mode = :recursive, &block)
	    TranslationUnit.each_(@declarations, meta, mode, &block)
    end

    private
    def TranslationUnit.each_(ary, meta, mode, &block)
	    puts("Starting each_")
	    ary.each do |decl|
		    case decl
		    when Hash
			    if decl.has_key?(:meta)
				    case decl[:meta]
				    when meta
					    yield(decl)
				    when :namespace
					    TranslationUnit.each_(decl[:declarations], meta, mode, &block) if mode == :recursive
				    end
			    end
		    when Array
			    TranslationUnit.each_(decl, meta, mode, &block) if mode == :recursive
		    end
	    end
	    puts("Ending each_")
    end
    @@reUsing = /using namespace [^;]+;/
    @@reUndef = /\#undef .+$/
    @@rePragma = /\#pragma .+$/
    @@reExtern = /extern .+$/
    @@reTypedef = /\s*typedef\s+.+;/
    @@reSemiCol = /[^;(){}]*;[\n\r]+/
        def TranslationUnit.parseDeclarations_(token, open = nil, close = nil)
            declarations = []
	    if open
		    declarations << TranslationUnit.parseBrol_(token)
		    o = token.popFront(open)
		    return nil if o.nil?
		    declarations << o
	    end
            while !token.empty?
                log("\nrest(#{token.length}): (((#{token.hexdump($dumpSize)})))")
		if close
			if c = token.popFront(close)
				declarations << c
				break
			end
		end
		if newline = token.popNewline
			log("Found newline")
			declarations << newline
                elsif inc = token.popFront{|token|TranslationUnit.parseInclude_(token)}
			log("Found include #{inc}")
			declarations << inc
                elsif using = token.popFront(@@reUsing)
                    log("Found using #{using}")
                    declarations << using
                elsif ppIf = token.popFront{|token|TranslationUnit.parsePPIf_(token)}
                    log("Found pp if of length #{ppIf.length}")
                    declarations << ppIf
                elsif ppDefine = token.popFront{|token|TranslationUnit.parseDefine_(token)}
                    log("Found a define #{ppDefine}")
                    declarations << ppDefine
                elsif ppPragma = token.popFront(@@rePragma)
                    log("Found a pragme #{ppPragma}")
                    declarations << ppPragma
                elsif ppUndef = token.popFront(@@reUndef)
                    log("Found an undef #{ppUndef}")
                    declarations << ppUndef
                elsif extern = token.popFront(@@reExtern)
                    log("Found an extern #{extern}")
                    declarations << extern
                elsif td = token.popFront(@@reTypedef)
                    log("Found a typedef #{td}")
                    declarations << td
                elsif namespace = token.popFront{|token|TranslationUnit.parseNamespace_(token)}
                    log("Found a namespace")
                    declarations << namespace
                elsif klass = token.popFront{|token|TranslationUnit.parseClassDefinition_(token)}
                    log("Found a struct or class definition of length #{klass.length}")
                    declarations << klass
                elsif templdefdecl = token.popFront{|token|TranslationUnit.parseTemplFunctionDefDecl_(token)}
                    log("Found a function templdefdecl of length #{templdefdecl.length}")
                    declarations << templdefdecl
                elsif defdecl = token.popFront{|token|TranslationUnit.parseFunctionDefDecl_(token)}
                    log("Found a function defdecl of length #{defdecl.length}")
                    declarations << defdecl
                elsif macroCall = token.popFront{|token|TranslationUnit.parseMacroCall_(token)}
                    log("Found a macro call")
                    declarations << macroCall
                elsif function = token.popFront{|token|TranslationUnit.parseFunctionDefinitionReturningFunctionPointer_(token)}
                    log("Found a function definition of length #{function.length}")
                    declarations << function
                elsif array = token.popFront{|token|TranslationUnit.parseArrayDefinition_(token)}
                    log("Found an array definition")
                    declarations << array
                elsif variable = token.popFront{|token|TranslationUnit.parseVariableDefinition_(token)}
                    log("Found a variable definition")
                    declarations << variable
                elsif comment = token.popFront{|token|TranslationUnit.parseComment_(token)}
                    log("Found a comment")
                    declarations << comment
                elsif semicol = token.popFront(@@reSemiCol)
                    log("Found something with a semicol")
                    declarations << semicol
		elsif brol = TranslationUnit.parseBrol_(token)
                    log("Found some brol")
                    declarations << brol
                else
			raise("Could not parse (((#{token.hexdump($dumpSize)})))")
                end
            end
	    declarations
        end
    def TranslationUnit.generate_(ary)
        ary.map do |decl|
            case decl
            when Array
                TranslationUnit.generate_(decl)
            when Hash
                TranslationUnit.generate_(decl.values)
            when Symbol
                #This is meant for meta-information passing and won't be serialized
                ""
            else
                decl.to_s
            end
        end.join("")
    end
    @@reInclude = /\#include\s+["<]([^"<>]+)[">]/
    def TranslationUnit.parseInclude_(token)
	    inc = token.popFront(@@reInclude)
	    return nil if inc.nil?
	    {meta: :include, inc: inc}
    end
    @@rePPIf = /\#if.+$/
        @@reLine = /.+$/
        @@rePPEndif = /\#endif/
        def TranslationUnit.parsePPIf_(token)
            ppIf = token.popFront(@@rePPIf)
            return nil if !ppIf
            res = [ppIf]
            verbose = false
            loop do
                log("\ntoken(#{token.length}): #{token.hexdump($dumpSize)}") if verbose
                if newline = token.popNewline
                    res << newline
                elsif ppEndif = token.popFront(@@rePPEndif)
                    res << ppEndif
                    break
                elsif line = token.popFront(@@reLine)
                    res << line
                else
                    raise("Could not parse #{token.hexdump($dumpSize)}")
                end
            end
            log("Matched ppif #{res.length}") if verbose
            res
        end
        @@reDefine = /\#define\s+([A-Za-z_][A-Za-z_\d]*)/
	@@reDefineCont = /.*\\\s*$/
	@@reDefineLast = /.*$/
	def TranslationUnit.parseDefine_(token)
		define = token.popFront(@@reDefine)
		return nil if define.nil?
		log("Found \#define #{define}")
		rest = []
		while r = token.popFront(@@reDefineCont)
			log("\n\ncont\n\n")
			rest << r
			rest << token.popNewline
		end
		rest << token.popFront(@@reDefineLast)
		{define: define, rest: rest}
	end
	@@reMacroCall = /([A-Z][A-Z_]+)/
        def TranslationUnit.parseMacroCall_(token)
		macro = token.popFront(@@reMacroCall)
		return nil if macro.nil? 
		log("macro: #{macro}")
		parameters = TranslationUnit.parseScope_(token, "(", ")")
		return nil if parameters.nil? 
		body = TranslationUnit.parseScope_(token, "{", "}")
		{macro: macro, parameters: parameters, body: body}
        end
    @@reNamespace = /namespace\s+([a-zA-Z_]+)\s*/
        @@reNamespaceAnon = /namespace\s*/
        def TranslationUnit.parseNamespace_(token)
            ns = nil
            if ns = token.popFront(@@reNamespace)
            elsif ns = token.popFront(@@reNamespaceAnon)
            else
                return nil
            end
	    log("Found a namespace #{ns}")
	    declarations = TranslationUnit.parseDeclarations_(token, "{", "}")
	    raise("I expected to find declarations") if declarations.nil?
            {meta: :namespace, ns: ns, declarations: declarations}
        end
	@@reStructClass = /(struct|class)\s+/
	@@reSCName = /([A-Za-z][A-Za-z_\d]*::)*[A-Za-z][A-Za-z_\d]*\s*/
	@@reBase = /((public|private)\s+)?(::)?([A-Za-z][A-Za-z_\d]*::)*[A-Za-z][A-Za-z_\d]*\s*/
	def TranslationUnit.parseClassDefinition_(token)
		brol = TranslationUnit.parseBrol_(token)
		sc = token.popFront(@@reStructClass)
		return nil if sc.nil?
		name = token.popFront(@@reSCName)
		log("struct or class name: #{name}")
		colon, bases = nil, []
		if colon = token.popFront(/\s*:\s*/)
			while base = token.popFront(@@reBase)
				bases << base
			end
		end
		body = TranslationUnit.parseScope_(token, "{", "}")
		semicol = token.popFront(";")
		{brol: brol, sc: sc, name: name, colon: colon, bases: bases, body: body, semicol: semicol}
	end
	@@reSpecifier = /(static|const)\s+/
	@@reBasicType = /(void|(unsigned\s+)?(char|short|int|long))\b\s*/
	@@reUserType = /(typename\s+)?(::|([A-Za-z_]+::)*([A-Za-z_][A-Za-z_\d]*))\b\s*/
	def TranslationUnit.parseType_(token)
		specifier = token.popFront(@@reSpecifier)
		type = token.popFront(@@reBasicType) || token.popFront(@@reUserType)
		return nil if type.nil?
		template = TranslationUnit.parseScope_(token, "<", ">")
		{specifier: specifier, type: type, template: template}
	end
    @@reCtor = /([A-Z][A-Za-z_\d]*::)*([A-Z][A-Za-z_\d]*)::\2/
        @@reDtor = /([A-Z][A-Za-z_\d]*::)*([A-Z][A-Za-z_\d]*)::~\2/
	@@reReturnTypeMod = /[&\*]+\s*/
        @@reMethod = /([A-Za-z][A-Za-z_\d]*::)+([A-Za-z_\d]+)/
        @@reOperator = /([A-Z][A-Za-z_\d]*::)*operator(<<|>>|\|\||&&|<=|>=|\+=|-=|\*=|\/=|!=|\+\+|--|\(\)|\[\]|==|<|>|=|\+|-|\*|\/)/
        @@reFunction = /[a-zA-Z][a-zA-Z\d_<]+/
	@@reConst = /\s*const\s*/
        def TranslationUnit.parseFunctionDefDecl_(token)
            brol = TranslationUnit.parseBrol_(token)
            returnType, retvm, retvcomment, name = nil, nil, nil, nil
            isCtor, isMethod = false, false
            if name = token.popFront(@@reCtor)
                isCtor, isMethod = true, true
            elsif name = token.popFront(@@reDtor)
                isMethod = true
            elsif returnType = TranslationUnit.parseType_(token)
		    log("Found a returnType: #{returnType}")
		    retvm = token.popFront(@@reReturnTypeMod)
		    retvcomment = TranslationUnit.parseComment_(token)
		    log("retvcomment: #{retvcomment}")
		    name = token.popFront(@@reOperator) || token.popFront(@@reMethod) || token.popFront(@@reFunction)
		    log("name: #{name}")
            end
            return nil if !name
	    log("Found a name: #{name}")
            brol1 = TranslationUnit.parseBrol_(token)
            parameters = TranslationUnit.parseScope_(token, "(", ")")
            return nil if parameters.nil?
            log("A function definition was found: #{name} (isCtor: #{isCtor})")
            initlist, const = nil, nil
            if isCtor
                initlist = token.popFront{|t|TranslationUnit.parseInitList_(t)}
	    else
		    const = token.popFront(@@reConst)
            end
            comment = TranslationUnit.parseComment_(token)
            body, semicolon = nil, nil
	    if body = TranslationUnit.parseScope_(token, "{", "}")
	    else
		    if semicolon = token.popFront(";")
		    else
			    raise("I expected to find either a body or a semicolon")
		    end
	    end
            {meta: :function, brol: brol, returnType: returnType, retvm: retvm, retvcomment: retvcomment, name: name, brol1: brol1, parameters: parameters, initlist: initlist, const: const, comment: comment, body: body, semicolon: semicolon}
        end
	@@reTemplate = /template\s+/
	def TranslationUnit.parseTemplFunctionDefDecl_(token)
		template = token.popFront(@@reTemplate)
		return nil if template.nil?
		log("Found a template")
		templateArgs = TranslationUnit.parseScope_(token, "<", ">")
		raise("Expected template arguments") if templateArgs.nil?
		function = TranslationUnit.parseFunctionDefDecl_(token)
		raise("Expected a function") if function.nil?
		{template: template, templateArgs: templateArgs, function: function}
	end
        def TranslationUnit.parseFunctionDefinitionReturningFunctionPointer_(token)
            brol = TranslationUnit.parseBrol_(token)
            returnType = TranslationUnit.parseType_(token)
	    return nil if returnType.nil?
	    retvm = token.popFront(@@reReturnTypeMod)
	    open = token.popFront("(")
	    return nil if open.nil?
	    star = token.popFront("*")
	    name = token.popFront(@@reMethod)
	    return nil if name.nil?
	    log("Found function returning #{name}")
	    args = TranslationUnit.parseScope_(token, "(", ")")
	    close = token.popFront(")")
	    return nil if close.nil?
	    argsPtr = TranslationUnit.parseScope_(token, "(", ")")
	    raise("I expected to find args") if args.nil?
	    body = TranslationUnit.parseScope_(token, "{","}")
	    raise("I expected to find a body") if body.nil?
            {meta: :function, brol: brol, returnType: returnType, retvm: retvm, open: open, star: star, name: name, args: args, close: close, argsPtr: argsPtr, body: body}
        end
	@@reVarSpecifiers = /((const|static)\s+)+/
        @@reVarType = /([A-Za-z_][A-Za-z_\d:]*)\s*/
        @@reVarTypeMod = /[&\*]+\s*/
        @@reVarName = /([A-Za-z_][A-Za-z_\d]*)\s*/
        @@reVarRest = /[^;]*;/m
        def TranslationUnit.parseArrayDefinition_(token)
            brol = TranslationUnit.parseBrol_(token)
	    specifiers = token.popFront(@@reVarSpecifiers)
	    type = token.popFront(@@reVarType)
	    return nil if type.nil?
	    typem = token.popFront(@@reVarTypeMod)
	    name = token.popFront(@@reVarName)
	    return nil if name.nil?
	    dim = TranslationUnit.parseScope_(token, "[", "]")
	    return nil if dim.nil?
	    rest = token.popFront(@@reVarRest)
	    return nil if rest.nil?
	    log("rest: #{rest}")
            {meta: :variable, brol: brol, specifiers: specifiers, type: type, typem: typem, name: name, dim: dim, rest: rest}
        end
        def TranslationUnit.parseVariableDefinition_(token)
            brol = TranslationUnit.parseBrol_(token)
	    specifiers = token.popFront(@@reVarSpecifiers)
	    type = token.popFront(@@reVarType)
	    return nil if type.nil?
	    typem = token.popFront(@@reVarTypeMod)
	    name = token.popFront(@@reVarName)
	    return nil if name.nil?
	    rest = token.popFront(@@reVarRest)
	    return nil if rest.nil?
            {meta: :variable, brol: brol, specifiers: specifiers, type: type, typem: typem, name: name, rest: rest}
        end
    def TranslationUnit.parseInitList_(token)
        brol = TranslationUnit.parseBrol_(token)
        dd = token.popFront(":")
        return nil if dd.nil?
        inits = token.popUntil("{")
        [brol, dd, inits]
    end
    def TranslationUnit.parseScope_(token, ot, ct)
        verbose = false
        log("Parsing scope #{ot} #{ct}") if verbose
        brolBefore = TranslationUnit.parseBrol_(token)
        c = token.content
        six, eix = 0, 0
        oix = c.index(ot, eix)
        return nil if oix != 0
        cix = c.index(ct, eix)
        level = 0
        loop do
            raise("Could not find closing ix for #{ct}") if cix.nil?
            if oix.nil?
                level -= 1
                eix = cix+ct.length
                cix = c.index(ct, eix)
            else
                if oix < cix
                    level += 1
                    eix = oix+ot.length
                    oix = c.index(ot, eix)
                else
                    level -= 1
                    eix = cix+ct.length
                    cix = c.index(ct, eix)
                end
            end
            if level == 0
                eix -= ct.length
                break
            end
        end
        res = {}
        res[:brolBefore] = brolBefore
        res[:open] = token.popFront(ot)
        res[:middle] = token.popFront(c[ot.length, eix-ot.length])
        res[:close] = token.popFront(ct)
	res[:brolAfter] = TranslationUnit.parseBrol_(token)
        log("Parsing scope ok: #{res[:middle]}") if verbose
        res
    end
    @@reWhitespace = /\s+/
        def TranslationUnit.parseBrol_(token)
            res = []
            loop do
                if nl = token.popNewline
                    res << nl
                elsif ws = token.popFront(@@reWhitespace)
                    res << ws
                else
                    return res.empty? ? nil : res
                end
            end
        end
    @@reNewstyleComment = /\/\/.+$/
        def TranslationUnit.parseComment_(token)
            res = nil
            token.transaction do
                brol = TranslationUnit.parseBrol_(token)
                if oldStyle = parseScope_(token, "/*", "*/")
                    log("Old-style comment")
                    res = {brol: brol, comment: oldStyle}
                    :ok
                elsif newStyle = token.popFront(@@reNewstyleComment)
                    log("New-style comment")
		    newline = token.popNewline
                    res = {brol: brol, comment: newStyle, newline: newline}
                    :ok
                else
                    :failure
                end
            end
            res
        end
end
