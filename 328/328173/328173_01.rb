def A(m, d, n)
  str = (n * n).to_s
  s = str.size
  (0..s - 2).each{|i|
    j, k = str[0..i].to_i, str[i + 1..-1].to_i
    s_str = (j + k).to_s
    if s_str.split('').uniq.size <= m
      #print "#{n}^2 = #{str.rjust(2 * d)},"
print "#{n} "
      puts " #{s_str.rjust(d + 1)} = #{j.to_s.rjust(i + 1 + 2 * d - s)} + #{k.to_s.rjust(s - i - 1)}"
    end
  }
end

#(1..10 ** 6 - 1).each{|i| A(1, i.to_s.size, i)}
(1..27).each{|i|
  k = (10 ** i - 1) / 9
  (1..9).each{|j|
    A(1, i, j * k)
  }
}
