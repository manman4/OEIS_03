def generate_132_avoiders(n, memo)
  cached = memo[n]
  return cached unless cached.nil?

  if n == 0
    memo[0] = [[]]
    return memo[0]
  end

  ary = []
  0.upto(n - 1){|left_size|
    right_size = n - 1 - left_size
    left_list = generate_132_avoiders(left_size, memo)
    right_list = generate_132_avoiders(right_size, memo)

    left_list.each{|left_perm|
      shifted_left = left_perm.map{|v| v + right_size}
      right_list.each{|right_perm|
        ary << (shifted_left + [n - 1] + right_perm)
      }
    }
  }
  memo[n] = ary
end

def order_divides_3?(perm)
  n = perm.length
  used = Array.new(n, false)

  0.upto(n - 1){|start|
    next if used[start]

    len = 0
    i = start
    until used[i]
      used[i] = true
      i = perm[i]
      len += 1
      return false if len > 3
    end
    return false unless len == 1 || len == 3
  }
  true
end

def count(n, memo = {})
  generate_132_avoiders(n, memo).count{|perm| order_divides_3?(perm)}
end

def sequence(limit)
  raise ArgumentError, "limit must be nonnegative" if limit < 0

  memo = {}
  (0..limit).map{|n| count(n, memo)}
end

if __FILE__ == $PROGRAM_NAME
  limit = (ARGV[0] || 12).to_i
  ary = sequence(limit)
  puts ary.join(", ")
  ary.each_with_index{|v, n|
    puts "#{n} #{v}"
  }
end
