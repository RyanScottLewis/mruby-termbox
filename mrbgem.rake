MRuby::Gem::Specification.new('mruby-termbox') do |spec|
  spec.authors = 'Ryan Scott Lewis <ryanscottlewis@lewis-software.com>'
  spec.summary = 'Termbox binding.'
  spec.license = 'MIT'
  spec.version = '0.1.0'
  
  spec.linker.libraries << 'termbox'
  
  spec.rbfiles << "#{dir}/mrblib/termbox/event.rb"
end
