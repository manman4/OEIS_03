n = 1000
ary = []
(1..n).each{|a|
  (a + 1..n).each{|b|
    (b + 1..n).each{|c|
      ary << c * (a + 1) + a * (b + 1) + b * (c + 1)
    }
  }
}
a = ary.uniq.sort

p (1..50000).to_a - a 
