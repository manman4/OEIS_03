def A130152(n)
  ary = Array.new(n, 0)
  (1..n).to_a.permutation{|a|
    ary[(1..n).map{|i| (a[i - 1] - i).abs}.max] += 1
  }
  ary
end

(1..9).each{|i| p A130152(i)}


def A364817(n)
  ary = Array.new(n, 0)
  (1..n).to_a.permutation{|a|
    ary[(1..n).map{|i| (a[a[i - 1] - 1] - i).abs}.max] += 1
  }
  ary
end

p (1..10).map{|i| A364817(i)}.flatten
(1..9).each{|i| p A364817(i).inject(:+)}
