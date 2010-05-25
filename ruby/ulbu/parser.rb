require "objects"

module UlbuParser
	def setBaseName(baseName)
		@baseName = baseName
	end

	def whitespaces
		[" ", "\n", "\t"]
	end
	def prepareParsing
	end

	def complete4Parser(obj)
		case obj

		when NilClass
			cota = Cota.new
			cota.name = Name.new(@baseName)
			cota.scope = complete(Scope.new)
			return cota

		when Cota
			cota = obj
			# Read the attributes
			cota.attributes = complete(Attributes.new)
			# Read the name
			cota.name = complete(Name.new)
			puts("Parsing #{cota.name}")
			if !matches(/\A[ \n]*:/)
				raise "ERROR::Could not read the name"
			end

			if cota.attributes.directive?
				# Read the directive as-is
				if !matches("{", "}") do
					cota.scope = buffer[0, buffer.index("}")]
					consume(cota.scope.length)
				end
				raise "ERROR::Could not create directive"
				end        
			else
				# Read the scope
				if matches("{", "}") do
					cota.scope = complete(Scope.new)
				end
				else
					cota.refCota = Cota.new
					cota.refCota.name = complete(Name.new)
					raise "ERROR::Could not create the refCota" if cota.refCota.name.nil?
				end
			end
			return cota

		when Scope
			scope = obj
			loop do
				break if (buffer[/\A[ \n]*\}/] or buffer[/\A[ \n]*\z/])
				scope.cotas << complete(Cota.new)
			end
			return scope

		when Attributes
			attributes = obj
			if !matches(/\A[ \n]*([@\$\+\-\.\/]*)/) do |attr|
				attributes.set(attr)
			end
			raise "ERROR::Could not read the attributes"
			end
			return attributes

		when Name
			name = obj
			if !matches(/\A[ \n]*([a-zA-Z\.]+)/) do |n|
				name = n
			end
			raise "ERROR::Could not complete the name"
			end
			return name

		end
		nil
	end

	def finishedParsing
	end
end
