ary =
[33,99,1119111,10110901101,11110901111,
 101109901101,1011109011101,1110109010111,
 10111099011101,100110090011001,100111090111001,
 101011090110101,111001090100111]
p a = ary.map{|i| i * i}
p b = a.map{|i| Math.sqrt(i.to_s.reverse.to_i).round}

def pl(n)
  n.to_s.reverse.to_i == n
end
p b.map{|i| pl(i)}
