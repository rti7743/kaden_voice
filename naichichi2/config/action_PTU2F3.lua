function call(iremocon_number,iremocon_onoff)
	execute("PTU2F3Command.exe","" .. iremocon_number .. " " .. iremocon_onoff);
end
