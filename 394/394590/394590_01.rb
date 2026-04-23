def a(n)
  m = 2 * n
  base = (1..m).to_a

  base.permutation.count{|perm|
    moved_left = 0
    moved_right = 0

    perm.each_with_index{|value, idx|
      pos = idx + 1
      if pos < value
        moved_left += 1
      elsif pos > value
        moved_right += 1
      end
    }

    moved_left - moved_right == n
  }
end

n_max = 5
0.upto(n_max){|n|
  puts "#{n} #{a(n)}"
}
