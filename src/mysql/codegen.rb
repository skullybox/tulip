require 'pry'

def parse_file
  begin
    f = File.open('objects.txt','r')
  rescue
    puts "File IO error!"
    throw 1
  end

  while  line = f.gets 
    table = line.split(':').first
    raw_cols = line.split(':').last.split(',')
    types = [] # 0 = int; 1 = bool; 2 = string
    cols = []

    raw_cols.each do |x|
      if x.index('(') == nil
        types.push(0)
        cols.push(x)
      end
      if x.index('(') != nil
        cols.push(x.split('(').first)
        strsz = x.match( /\([0-9]+\)/ )
        strsz = strsz.to_s.split('(').last
        strsz = strsz.split(')').first

        if strsz == "1"
          types.push(1)
        else
          types.push(2)
        end
      end

      # print definition
      counter = 0
      cols.each do |x|
        begin
          hf = File.open("g_mysql_sql_#{table}_#{x}.h",'w')
        rescue
          puts "File IO error!"
          throw 1
        end

        if types[counter] == 0
          hf.write("/* mysql object func */\n")
          hf.write("g_mysql_sql_#{table}_#{x}( unsigned p);\n")
        elsif types[counter] == 1
          hf.write("/* mysql object func */\n")
          hf.write("g_mysql_sql_#{table}_#{x}( char p);\n")
        else
          hf.write("/* mysql object func */\n")
          hf.write("g_mysql_sql_#{table}_#{x}( char *p);\n")
        end
        counter += 1
      end


      # print implementation
      counter = 0
      cols.each do |x|
        begin
          hc = File.open("g_mysql_sql_#{table}_#{x}.c",'w')
        rescue
          puts "File IO error!"
          throw 1
        end

        if types[counter] == 0
          hc.write("g_mysql_sql_#{table}_#{x}( u p)\n{")
        elsif types[counter] == 1
          hc.write("g_mysql_sql_#{table}_#{x}( c p)\n{")
        else
          hc.write("g_mysql_sql_#{table}_#{x}( c *p)\n{")
        end

        hc.write("  char *query = NULL;")
        if types[counter] == 0
          hc.write("  char sql[]= \"select * from #{table} where #{x} == p;\"")
        elsif types[counter] == 1
          hc.write("  char sql[]= \"select * from #{table} where #{x} == p;\"")
        elsif types[counter] == 2
          hc.write("  char sql[]= \"select * from #{table} where #{x} like '%\";")
        end
          hc.write("  query = (char*)malloc(strlen(sql)+1strlen(p)+10);")
          hc.write("  strcpy(query,sql);")
          if types[counter] == 2
            hc.write("  strcat(query,p);")
            hc.write("  strcat(query,\"%';\");")
          end

        counter += 1
        hc.write("}\n")
      end
    end 

  end
end


begin
  parse_file
rescue
  exit 1
end



