#include <stdlib.h>

#include <mruby.h>
#include <mruby/variable.h>

#include <termbox.h>

// HELPER
// Create and return an MRuby Termbox::Event instance from a given tb_event struct.
mrb_value mrb_mruby_termbox_event_from_struct( mrb_state *mrb, struct tb_event *tb_event ) {
  mrb_value mrb_event = mrb_obj_new( mrb, mrb_class_get_under( mrb, mrb_module_get( mrb, "Termbox" ), "Event" ), 0, NULL );
  
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@type" ), mrb_fixnum_value( tb_event->type ) );
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@modifier" ), mrb_fixnum_value( tb_event->mod ) );
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@key" ), mrb_fixnum_value( tb_event->key ) );
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@character" ), mrb_fixnum_value( tb_event->ch ) );
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@width" ), mrb_fixnum_value( tb_event->w ) );
  mrb_iv_set( mrb, mrb_event, mrb_intern_cstr( mrb, "@height" ), mrb_fixnum_value( tb_event->h ) );
  
  return mrb_event;
}

mrb_value mrb_mruby_termbox_change_cell_module_method( mrb_state *mrb, mrb_value self ) {
  mrb_int mrb_x, mrb_y, mrb_character, mrb_foreground, mrb_background;
  mrb_get_args( mrb, "iiiii", &mrb_x, &mrb_y, &mrb_character, &mrb_foreground, &mrb_background );

  // tb_change_cell( mrb_x, mrb_y, (uint32_t)mrb_character, (uint16_t)mrb_foreground, (uint16_t)mrb_background );
  tb_change_cell( mrb_x, mrb_y, mrb_character, mrb_foreground, mrb_background );

  return self;
}

// Clears the internal back buffer using TB_DEFAULT color or the color/attributes set by Termbox.set_clear_attributes() method.
mrb_value mrb_mruby_termbox_clear_module_method( mrb_state *mrb, mrb_value self ) {
  tb_clear();

  return self;
}

mrb_value mrb_mruby_termbox_height_module_method( mrb_state *mrb, mrb_value self ) {
  return mrb_fixnum_value( tb_height() );
}

// Initializes the termbox library.
// This function should be called before any other functions.
// After successful initialization, the library must be finalized using the Termbox.shutdown method.
mrb_value mrb_mruby_termbox_init_module_method( mrb_state *mrb, mrb_value self ) {
  int code = tb_init();

  if( code < 0 ) {
    struct RClass *mrb_mruby_termbox_module = mrb_class_get( mrb, "Termbox" );

    switch( code ) {
    case -1:
      mrb_raise( mrb, mrb_class_get_under( mrb, mrb_mruby_termbox_module, "UnsupportedTerminalError" ), "Unsupported terminal" );
    case -2:
      mrb_raise( mrb, mrb_class_get_under( mrb, mrb_mruby_termbox_module, "FailedToOpenTTYError" ), "Failed to open TTY" );
    case -3:
      mrb_raise( mrb, mrb_class_get_under( mrb, mrb_mruby_termbox_module, "PipeTrapError" ), "Pipe trap error" );
    }

    if( code > -3 ) // TODO: I don't think this makes sense
      mrb_raise( mrb, mrb_class_get_under( mrb, mrb_mruby_termbox_module, "Error" ), "Could not be initialized" );
  }

  return mrb_true_value();
}

mrb_value mrb_mruby_termbox_output_mode_getter_module_method( mrb_state *mrb, mrb_value self ) {
  return mrb_fixnum_value( tb_select_output_mode( TB_OUTPUT_CURRENT ) );
}

mrb_value mrb_mruby_termbox_output_mode_setter_module_method( mrb_state *mrb, mrb_value self ) {
  mrb_int mrb_mode;
  mrb_get_args( mrb, "i", &mrb_mode );

  return mrb_fixnum_value( tb_select_output_mode( mrb_mode ) );
}

// Wait for an event for a given amount of time in milliseconds.
mrb_value mrb_mruby_termbox_peek_event_module_method( mrb_state *mrb, mrb_value self ) {
  struct tb_event event;
  mrb_int timeout;

  mrb_get_args( mrb, "i", &timeout );

  int code = tb_peek_event( &event, timeout );

  if( code < 0 )
    mrb_raise( mrb, mrb_class_get_under( mrb, mrb_module_get( mrb, "Termbox" ), "Error" ), "Could not peek events" );

  return mrb_mruby_termbox_event_from_struct( mrb, &event );
}

// Wait for an event forever.
mrb_value mrb_mruby_termbox_poll_event_module_method( mrb_state *mrb, mrb_value self ) {
  struct tb_event event;
  struct RClass *mrb_mruby_termbox_module = mrb_module_get( mrb, "Termbox" );

  int code = tb_poll_event( &event );

  if( code < 0 )
    mrb_raise( mrb, mrb_class_get_under( mrb, mrb_mruby_termbox_module, "Error" ), "Could not poll events" );

  return mrb_mruby_termbox_event_from_struct( mrb, &event );
}

// Syncronizes the internal back buffer with the terminal.
mrb_value mrb_mruby_termbox_present_module_method( mrb_state *mrb, mrb_value self ) {
  tb_present();

  return self;
}

mrb_value mrb_mruby_termbox_set_clear_attributes_module_method( mrb_state *mrb, mrb_value self ) {
  mrb_int mrb_foreground, mrb_background;
  mrb_get_args( mrb, "ii", &mrb_foreground, &mrb_background );

  tb_set_clear_attributes( (uint16_t)mrb_foreground, (uint16_t)mrb_background );

  return self;
}

// Set the position of the cursor.
// If Termbox::HIDE_CURSOR is passed as both coordinates, then the cursor will be hidden.
// Cursor is hidden by default.
mrb_value mrb_mruby_termbox_set_cursor_module_method( mrb_state *mrb, mrb_value self ) {
  mrb_int mrb_x, mrb_y;
  mrb_get_args( mrb, "ii", &mrb_x, &mrb_y );

  tb_set_cursor( mrb_x, mrb_y );

  return self;
}

mrb_value mrb_mruby_termbox_shutdown_module_method( mrb_state *mrb, mrb_value self ) {
  tb_shutdown();

  return self; // TODO: Should this be smarter by checking is already initialized?
}

mrb_value mrb_mruby_termbox_utf8_char_to_unicode_module_method( mrb_state *mrb, mrb_value self ) {
  char *str;
  mrb_get_args( mrb, "z", &str );
  
  uint32_t unicode;
  tb_utf8_char_to_unicode( &unicode, str );
  
  return mrb_fixnum_value( unicode );
}

mrb_value mrb_mruby_termbox_width_module_method( mrb_state *mrb, mrb_value self ) {
  return mrb_fixnum_value( tb_width() );
}

void mrb_mruby_termbox_gem_init( mrb_state *mrb ) {
  struct RClass *mrb_mruby_termbox_module = mrb_define_module( mrb, "Termbox" );

  // Event constants
  mrb_define_const( mrb, mrb_mruby_termbox_module, "EVENT_KEY", mrb_fixnum_value( TB_EVENT_KEY ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "EVENT_RESIZE", mrb_fixnum_value( TB_EVENT_RESIZE ) );

  // Color constants
  mrb_define_const( mrb, mrb_mruby_termbox_module, "DEFAULT", mrb_fixnum_value( TB_DEFAULT ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "BLACK", mrb_fixnum_value( TB_BLACK ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "RED", mrb_fixnum_value( TB_RED ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "GREEN", mrb_fixnum_value( TB_GREEN ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "YELLOW", mrb_fixnum_value( TB_YELLOW ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "BLUE", mrb_fixnum_value( TB_BLUE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "MAGENTA", mrb_fixnum_value( TB_MAGENTA ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "CYAN", mrb_fixnum_value( TB_CYAN ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "WHITE", mrb_fixnum_value( TB_WHITE ) );

  // Attribute constants
  mrb_define_const( mrb, mrb_mruby_termbox_module, "BOLD", mrb_fixnum_value( TB_BOLD ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "UNDERLINE", mrb_fixnum_value( TB_UNDERLINE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "REVERSE", mrb_fixnum_value( TB_REVERSE ) );

  // Output mode constants
  mrb_define_const( mrb, mrb_mruby_termbox_module, "OUTPUT_CURRENT", mrb_fixnum_value( TB_OUTPUT_CURRENT ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "OUTPUT_NORMAL", mrb_fixnum_value( TB_OUTPUT_NORMAL ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "OUTPUT_256", mrb_fixnum_value( TB_OUTPUT_256 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "OUTPUT_216", mrb_fixnum_value( TB_OUTPUT_216 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "OUTPUT_GRAYSCALE", mrb_fixnum_value( TB_OUTPUT_GRAYSCALE ) );

  // Key constants
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F1", mrb_fixnum_value( TB_KEY_F1 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F2", mrb_fixnum_value( TB_KEY_F2 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F3", mrb_fixnum_value( TB_KEY_F3 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F4", mrb_fixnum_value( TB_KEY_F4 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F5", mrb_fixnum_value( TB_KEY_F5 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F6", mrb_fixnum_value( TB_KEY_F6 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F7", mrb_fixnum_value( TB_KEY_F7 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F8", mrb_fixnum_value( TB_KEY_F8 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F9", mrb_fixnum_value( TB_KEY_F9 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F10", mrb_fixnum_value( TB_KEY_F10 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F11", mrb_fixnum_value( TB_KEY_F11 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_F12", mrb_fixnum_value( TB_KEY_F12 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_INSERT", mrb_fixnum_value( TB_KEY_INSERT ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_DELETE", mrb_fixnum_value( TB_KEY_DELETE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_HOME", mrb_fixnum_value( TB_KEY_HOME ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_END", mrb_fixnum_value( TB_KEY_END ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_PGUP", mrb_fixnum_value( TB_KEY_PGUP ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_PGDN", mrb_fixnum_value( TB_KEY_PGDN ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ARROW_UP", mrb_fixnum_value( TB_KEY_ARROW_UP ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ARROW_DOWN", mrb_fixnum_value( TB_KEY_ARROW_DOWN ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ARROW_LEFT", mrb_fixnum_value( TB_KEY_ARROW_LEFT ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ARROW_RIGHT", mrb_fixnum_value( TB_KEY_ARROW_RIGHT ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_TILDE", mrb_fixnum_value( TB_KEY_CTRL_TILDE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_2", mrb_fixnum_value( TB_KEY_CTRL_2 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_A", mrb_fixnum_value( TB_KEY_CTRL_A ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_B", mrb_fixnum_value( TB_KEY_CTRL_B ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_C", mrb_fixnum_value( TB_KEY_CTRL_C ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_D", mrb_fixnum_value( TB_KEY_CTRL_D ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_E", mrb_fixnum_value( TB_KEY_CTRL_E ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_F", mrb_fixnum_value( TB_KEY_CTRL_F ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_G", mrb_fixnum_value( TB_KEY_CTRL_G ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_BACKSPACE", mrb_fixnum_value( TB_KEY_BACKSPACE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_H", mrb_fixnum_value( TB_KEY_CTRL_H ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_TAB", mrb_fixnum_value( TB_KEY_TAB ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_I", mrb_fixnum_value( TB_KEY_CTRL_I ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_J", mrb_fixnum_value( TB_KEY_CTRL_J ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_K", mrb_fixnum_value( TB_KEY_CTRL_K ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_L", mrb_fixnum_value( TB_KEY_CTRL_L ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ENTER", mrb_fixnum_value( TB_KEY_ENTER ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_M", mrb_fixnum_value( TB_KEY_CTRL_M ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_N", mrb_fixnum_value( TB_KEY_CTRL_N ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_O", mrb_fixnum_value( TB_KEY_CTRL_O ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_P", mrb_fixnum_value( TB_KEY_CTRL_P ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_Q", mrb_fixnum_value( TB_KEY_CTRL_Q ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_R", mrb_fixnum_value( TB_KEY_CTRL_R ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_S", mrb_fixnum_value( TB_KEY_CTRL_S ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_T", mrb_fixnum_value( TB_KEY_CTRL_T ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_U", mrb_fixnum_value( TB_KEY_CTRL_U ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_V", mrb_fixnum_value( TB_KEY_CTRL_V ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_W", mrb_fixnum_value( TB_KEY_CTRL_W ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_X", mrb_fixnum_value( TB_KEY_CTRL_X ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_Y", mrb_fixnum_value( TB_KEY_CTRL_Y ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_Z", mrb_fixnum_value( TB_KEY_CTRL_Z ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_ESC", mrb_fixnum_value( TB_KEY_ESC ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_LSQ_BRACKET", mrb_fixnum_value( TB_KEY_CTRL_LSQ_BRACKET ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_3", mrb_fixnum_value( TB_KEY_CTRL_3 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_4", mrb_fixnum_value( TB_KEY_CTRL_4 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_BACKSLASH", mrb_fixnum_value( TB_KEY_CTRL_BACKSLASH ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_5", mrb_fixnum_value( TB_KEY_CTRL_5 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_RSQ_BRACKET", mrb_fixnum_value( TB_KEY_CTRL_RSQ_BRACKET ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_6", mrb_fixnum_value( TB_KEY_CTRL_6 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_7", mrb_fixnum_value( TB_KEY_CTRL_7 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_SLASH", mrb_fixnum_value( TB_KEY_CTRL_SLASH ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_UNDERSCORE", mrb_fixnum_value( TB_KEY_CTRL_UNDERSCORE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_SPACE", mrb_fixnum_value( TB_KEY_SPACE ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_BACKSPACE2", mrb_fixnum_value( TB_KEY_BACKSPACE2 ) );
  mrb_define_const( mrb, mrb_mruby_termbox_module, "KEY_CTRL_8", mrb_fixnum_value( TB_KEY_CTRL_8 ) );

  // Cursor options
  mrb_define_const( mrb, mrb_mruby_termbox_module, "HIDE_CURSOR", mrb_fixnum_value( TB_HIDE_CURSOR ) );

  // Errors
  struct RClass *mrb_mruby_termbox_error_class = mrb_define_class_under( mrb, mrb_mruby_termbox_module, "Error", E_RUNTIME_ERROR );
  mrb_define_class_under( mrb, mrb_mruby_termbox_module, "UnsupportedTerminalError", mrb_mruby_termbox_error_class );
  mrb_define_class_under( mrb, mrb_mruby_termbox_module, "FailedToOpenTTYError", mrb_mruby_termbox_error_class );
  mrb_define_class_under( mrb, mrb_mruby_termbox_module, "PipeTrapError", mrb_mruby_termbox_error_class );

  // Event
  mrb_define_class_under( mrb, mrb_mruby_termbox_module, "Event", mrb->object_class );

  // Methods
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "change_cell", mrb_mruby_termbox_change_cell_module_method, MRB_ARGS_REQ( 5 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "clear", mrb_mruby_termbox_clear_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "height", mrb_mruby_termbox_height_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "init", mrb_mruby_termbox_init_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "output_mode", mrb_mruby_termbox_output_mode_getter_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "output_mode=", mrb_mruby_termbox_output_mode_setter_module_method, MRB_ARGS_REQ( 1 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "peek_event", mrb_mruby_termbox_peek_event_module_method, MRB_ARGS_REQ( 1 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "poll_event", mrb_mruby_termbox_poll_event_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "present", mrb_mruby_termbox_present_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "set_clear_attributes", mrb_mruby_termbox_set_clear_attributes_module_method, MRB_ARGS_REQ( 2 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "set_cursor", mrb_mruby_termbox_set_cursor_module_method, MRB_ARGS_REQ( 2 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "shutdown", mrb_mruby_termbox_shutdown_module_method, MRB_ARGS_NONE() );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "utf8_char_to_unicode", mrb_mruby_termbox_utf8_char_to_unicode_module_method, MRB_ARGS_REQ( 1 ) );
  mrb_define_module_function( mrb, mrb_mruby_termbox_module, "width", mrb_mruby_termbox_width_module_method, MRB_ARGS_NONE() );
}

void mrb_mruby_termbox_gem_final( mrb_state *mrb ) {
}
