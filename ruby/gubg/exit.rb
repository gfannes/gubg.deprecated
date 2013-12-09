def finalize(msg, code = :error)
	prefix = (code == :error ? "ERROR::" : "")
	puts("#{prefix}#{msg}")
	exit(
		case code
		when :ok
			0
		when :error
			-1
		else
			-2
		end
	)
end
