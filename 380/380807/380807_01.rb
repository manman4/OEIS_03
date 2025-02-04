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

# 1..100のうちaに含まれない数を求める
p (1..50000).to_a - a 
