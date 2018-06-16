/*

  Copyright (c) 2018 SwissMicros GmbH

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_main.h"
#include "core_aux.h"


extern "C" {

#include <main.h>
#include <dmcp.h>

#include <dm42_menu.h>
#include <dm42_fns.h>


// Possibility to override default
#ifdef printf
//#undef printf
#endif


void after_fat_format_dm42() {
  if ( sys_disk_ok() ) {
    set_fat_label("DM42");
    check_create_dir(PGM_DIR);   // Create /PROGRAMS directory
    check_create_dir(STATE_DIR); // Create /STATE    directory
  }
}



/*
 ▄    ▄                               ▄▀▀                         ▄      ▀                        
 ██  ██  ▄▄▄   ▄ ▄▄   ▄   ▄         ▄▄█▄▄  ▄   ▄  ▄ ▄▄    ▄▄▄   ▄▄█▄▄  ▄▄▄     ▄▄▄   ▄ ▄▄    ▄▄▄  
 █ ██ █ █▀  █  █▀  █  █   █           █    █   █  █▀  █  █▀  ▀    █      █    █▀ ▀█  █▀  █  █   ▀ 
 █ ▀▀ █ █▀▀▀▀  █   █  █   █           █    █   █  █   █  █        █      █    █   █  █   █   ▀▀▀▄ 
 █    █ ▀█▄▄▀  █   █  ▀▄▄▀█           █    ▀▄▄▀█  █   █  ▀█▄▄▀    ▀▄▄  ▄▄█▄▄  ▀█▄█▀  █   █  ▀▄▄▄▀ */




int lcd_for_dm42(int what) {
  int refresh = 1;

  lcd_clear_buf();
  lcd_writeClr(t24);

  switch ( what ) {
    case DISP_CALC:
      refresh = 0; // Just empty screen
      break;

    case DISP_ALLOC_FAIL:
      lcd_putsR(t24, "Free42 Memory Allocation Fail");
      t24->y += 4;
      lcd_puts(t24, "Unhandled memory allocation:");
      break;

    case DISP_LOADING_STATE:
      lcd_putsR(t24, "Loading from File");
      lcd_putsAt(t24, 3, "Loading calculator state ...");
      break;

    case DISP_SAVING_STATE:
      lcd_putsR(t24, "Saving to File");
      lcd_putsAt(t24, 3, "Saving calculator state ...");
      break;

    case DISP_SAVE_FAILED:
      lcd_putsR(t24, "Save State Error");
      lcd_putsAt(t24, 2,"Calculator state save failed.");
      lcd_putsAt(t24, 3,"Check the FAT filesystem.");
      lcd_putsAt(t24, 4,"Disk full or too many");
      lcd_putsAt(t24, 5,"root directory items?");
      lcd_putsAt(t24, 7,"    Press EXIT key to continue...");
      break;

    case DISP_ABOUT:
      // Just base of original system about
      lcd_for_calc(DISP_ABOUT);
      lcd_putsAt(t24,4,"");
      lcd_prevLn(t24);
      // --
      
      int h2 = lcd_lineHeight(t20)/2;
      lcd_setXY(t20, t24->x, t24->y);
      t20->y += h2;
      lcd_print(t20, "DM42 v" DM42_VERSION " (C) SwissMicros GmbH");
      t20->y += h2;
      lcd_print(t20, "%s (C) 2004-2018, Thomas Okken", free42_version_str());
      t20->y += h2;
      lcd_puts(t20, "Intel Decimal Floating-Point Math Lib v2.0");
      lcd_puts(t20, "  (C) 2007-2011, Intel Corp.");

      t20->y = LCD_Y - lcd_lineHeight(t20);
      lcd_putsR(t20, "    Press EXIT key to continue...");
      break;

  }

  if (refresh)
    lcd_refresh();

  return t24->y;
}



#if 0 // Template screen
void template_screen() {
  char bb[32];
  font_t *f=font_lm18;;
  int k1,y=0;

  lcd_offset = 0;
  lcd_clear_buf();
  lcdPutsR("Screen title");
  
  for(;;) {
    k1 = runner_get_key(NULL);
    if ( k1 == KEY_EXIT ) break;
    if ( is_menu_auto_off() ) break;

    nr=-1;
    switch (k1) {
      case KEY_F1: // A - 
        break;
      case KEY_F2: // B - 
        break;
      case KEY_F3: // C - 
        break;
      case KEY_F4: // D - 
        break;
      case KEY_F5: // E - 
        break;
      case KEY_F6: // F - 
        break;
      case KEY_BSP: nr=10; break;
    }


    {
      
      // Menu line
      lcd_offset = 16;
      lcdPutsRAt(8, " xx | xx | xxx | xxx  | xxx | xx ");
      lcd_offset = 0;

      // display
      lcd_refresh();
    }
  }
}
#endif









int pgm_import_enter(const char * fpath, const char * fname, void * data) {
  FRESULT res;

  lcd_puts(t24,"Loading ...");
  lcd_puts(t24, fname);  lcd_refresh();
  res = f_open(ppgm_fp, fpath, FA_READ);
  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } else {

#ifdef FREE42_PRE_2016_IFC
    core_import_programs(NULL);
#else
    core_import_programs();
#endif

    f_close(ppgm_fp);
    if (pgm_res) {
      lcd_print(t24, "Fail - %i", pgm_res);
      lcd_refresh();
      wait_for_key_press();
    } else {
      lcd_puts(t24, "Success"); 
      lcd_refresh();
      sys_delay(1500);
    }
  }

  return 0; // Continue in the file list
}


typedef struct {
  int pgm_cnt;
  int pgm_indices[MAX_PGM_SEL];
} pgm_import_sel_t;


int pgm_export_enter(const char * fpath, const char * fname, void * data) {
  
  pgm_import_sel_t * pgm_sel = (pgm_import_sel_t*)data;
  int res;

  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,"Program Export");
  t24->ln_offs = 8;

  lcd_putsAt(t24,3,"Saving ...");
  lcd_puts(t24, fpath);
  lcd_refresh();

  sys_disk_write_enable(1);
  res = f_open(ppgm_fp, fpath, FA_WRITE|FA_CREATE_ALWAYS);
  if ( res != FR_OK ) {
    sys_disk_write_enable(0);
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    if ( sys_disk_ok() )
      wait_for_key_press();
  } else {
#ifdef FREE42_PRE_2016_IFC
    res = core_export_programs(pgm_sel->pgm_cnt, pgm_sel->pgm_indices, NULL);
#else
    core_export_programs(pgm_sel->pgm_cnt, pgm_sel->pgm_indices);
    res = 0;
#endif
    f_close(ppgm_fp);
    sys_disk_write_enable(0);
    if (res || pgm_res) {
      lcd_print(t24, "Fail - %i", pgm_res);
      lcd_refresh();
      if ( sys_disk_ok() )
        wait_for_key_press();
    } else {
      lcd_puts(t24,"Success"); 
      lcd_refresh();
      sys_delay(1000);
    }
  }

  return MRET_EXIT;
}


void run_reset_state_file() {
  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,"Calculator State");
  t24->ln_offs = 8;

  lcd_puts(t24, "You are about to reset calculator");
  lcd_puts(t24, "state.");
  lcd_puts(t24, "");
  lcd_puts(t24, "WARNING: Current calculator state");
  lcd_puts(t24, "will be lost.");
  lcd_puts(t24, "");
  lcd_puts(t24, "");
  //lcd_puts(t24, "Are you sure to proceed?");
  lcd_puts(t24, "Press [ENTER] to confirm.");
  lcd_refresh();
  
  wait_for_key_release(-1);

  for(;;) {
    int k1 = runner_get_key(NULL);
    if ( k1 == KEY_EXIT )
      return;
    if ( is_menu_auto_off() )
      return;
    if ( k1 == KEY_ENTER )
      break; // Proceed with reset
  }

  // Reset statefile name for next load
  set_reset_state_file("");

  // Reset the system to force new statefile load
  set_reset_magic(NO_SPLASH_MAGIC);
  sys_reset();
}


int load_statefile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24, "Calculator state file:");
  lcd_puts(t24, fname);

  // 'Sure' dialog
  lcd_puts(t24, "");
  lcd_puts(t24, "WARNING: Current calculator state");
  lcd_puts(t24, "will be lost.");
  lcd_puts(t24, "");
  lcd_puts(t24, "");
  //lcd_puts(t24, "Are you sure to load this file?");
  lcd_puts(t24, "Press [ENTER] to confirm.");
  lcd_refresh();
  
  wait_for_key_release(-1);

  for(;;) {
    int k1 = runner_get_key(NULL);
    if ( k1 == KEY_EXIT )
      return 0; // Continue the selection screen
    if ( is_menu_auto_off() )
      return MRET_EXIT; // Leave selection screen
    if ( k1 == KEY_ENTER )
      break; // Proceed with load
  }

  // Store statefile name for next load
  set_reset_state_file(fpath);

  // Reset the system to force new statefile load
  set_reset_magic(NO_SPLASH_MAGIC);
  sys_reset();

  // Never return here...
  return 0;
}


int save_statefile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24,"Saving state ...");
  lcd_puts(t24, fname);  lcd_refresh();

  // Store the state file name
  set_reset_state_file(fpath);

  // Exit with appropriate code to force statefile save
  return MRET_SAVESTATE;
}








STATIC_ASSERT( (AUX_BUF_PGM_LIST_SIZE+AUX_BUF_SELS_SIZE) <= AUX_BUF_SIZE, "(AUX_BUF_PGM_LIST_SIZE+AUX_BUF_SELS_SIZE) <= AUX_BUF_SIZE");


int select_programs(const char * title, int * pgm_indices, int * pgm_cnt) {
  char bb[32];
  int k1;
  int upd = 1; // Repaint at first pass

  // Disp state
  int first_ix = 0, cur_ix = 0, a, ix;
  int pix;  // Program index in display loop
  char * p; // Program name in display loop
  int sel_ln = 0;

  // We need rather big buffer ... use aux memory buffer for that
  const unsigned int pgm_list_size = AUX_BUF_PGM_LIST_SIZE;
  const unsigned int sels_size = AUX_BUF_SELS_SIZE;
  const unsigned int max_disp_pgm_char = 24;
  const int pgm_lines = 7;
  
  char * buf = aux_buf_ptr();
  char * sels = buf + pgm_list_size;

  // Read program list
  int pgm_count = core_list_programs(buf,pgm_list_size);

  // Clear selections
  *pgm_cnt = 0;
  bzero(sels, sels_size);

  // Display title
  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,title);
  t24->ln_offs = 8;

  for(;;) {
    k1 = runner_get_key(NULL);
    if ( k1 == KEY_EXIT ) break;
    if ( is_menu_auto_off() ) break;

    switch (k1) {
      case KEY_F1: // A - Sel
      case KEY_F6: // F - Sel
        if ( *pgm_cnt == MAX_PGM_SEL && !sels[cur_ix] ) {
          // Too much selections
          lcd_putsRAt(t24,sel_ln,"Max. nr. of selections is " STR(MAX_PGM_SEL));
          lcd_refresh();
          sys_delay(1000);
        } else {
          sels[cur_ix] = !sels[cur_ix];
          if(sels[cur_ix]) pgm_cnt[0]++; else pgm_cnt[0]--;
        }
        upd=1;
        break;
      case KEY_F2: // B - 
        break;
      case KEY_F3: // C - C-All
        // Clear selections
        *pgm_cnt = 0;
        bzero(sels,sels_size);
        upd=1;
        break;
      case KEY_F4: // D - S-All
        if ( pgm_count > MAX_PGM_SEL ) {
          lcd_putsRAt(t24,sel_ln,"Max. nr. of selections is " STR(MAX_PGM_SEL));
          lcd_refresh();
          sys_delay(1000);
        } else {
          *pgm_cnt = pgm_count;
          for(a=0; a<pgm_count; a++)
            sels[a] = 1;
        }
        upd=1;
        break;
      case KEY_F5: // E - 
        break;

      case KEY_UP:
        if (cur_ix > 0) {
          cur_ix--;
        } else {
          cur_ix = pgm_count-1;
        }
        upd=1;
        break;

      case KEY_DOWN:
        if (cur_ix+1 < pgm_count) {
          cur_ix++;
        } else {
          cur_ix = 0;
        }
        upd=1;
        break;

      case KEY_PAGEUP:
        if (cur_ix == 0) {
          cur_ix = pgm_count-1;
        } else {
          cur_ix--;
          cur_ix -= cur_ix % pgm_lines;
        }
        upd=1;
        break;

      case KEY_PAGEDOWN:
        if ( cur_ix == pgm_count-1) {
          cur_ix = 0;
        } else {
          cur_ix += pgm_lines;
          cur_ix -= cur_ix % pgm_lines;
          if ( cur_ix >= pgm_count )
            cur_ix = pgm_count-1;
          else
            first_ix = cur_ix;
        }
        upd=1;
        break;

      case KEY_ENTER:
        // Fill output data
        ix=0;
        for(a=0; a<pgm_count; a++)
          if( sels[a] )
            pgm_indices[ix++]=a;
        return 0;
    }


    if (upd) {
      upd = 0;

      // Update first_ix
      if (cur_ix < first_ix) first_ix = cur_ix;
      if (cur_ix - first_ix > (pgm_lines-1)) first_ix = cur_ix - (pgm_lines-1);
      if (first_ix + pgm_lines >= pgm_count ) first_ix = pgm_count - pgm_lines;
      if (first_ix < 0) first_ix = 0;

      p = buf; pix = 0; sel_ln = 0;
      for(a=0; a<pgm_lines && pix<pgm_count; pix++,p+=strlen(p)+1) { // Over lines
        if ( pix < first_ix ) continue;
        a++;

        // Selection mark
        strcpy(bb, sel_str(sels[pix]));
        //sprintf(bb+strlen(bb), "%02i", pix); // DEBUG :)
        strcat(bb, " ");
        // Program name (truncated to max_disp_pgm_char)
        if ( strlen(p) > max_disp_pgm_char ) {
          ix = strlen(bb);
          memcpy(bb+ix,p,max_disp_pgm_char);
          strcpy(bb+ix+max_disp_pgm_char, "...");
        } else {
          strcat(bb,p);
        }

        if ( pix == cur_ix ) {
          lcd_putsRAt(t24,a,bb);
          sel_ln = a;
        } else {
          lcd_putsAt(t24,a,bb);
        }

      }

      // DEBUG
      printf("pgm_cnt=%i  sel_ln=%i\n", *pgm_cnt, sel_ln);

      // Menu line
      //t24->y = LCD_Y - lcd_lineHeight(t24);
      //lcd_putsR(t24, " Sel |     |C-All|S-All|      | Sel");
      const char * menu[] = {" Sel","","C-All","S-All","","Sel"};
      lcd_draw_menu_keys(menu);

      // display
      lcd_refresh();
    }
  }
  return 1;
}


void pgm_export() {
  int res;

  // Don't pass through if the power is insufficient  
  if ( power_check_screen() )
    return;

  wait_for_key_release(-1);

  if ( check_create_dir(PGM_DIR) ) {
    lcd_puts(t24,"Cannot access " PGM_DIR); lcd_refresh();
    wait_for_key_press();
    return;
  }

  if ( !sys_disk_ok() ) goto pgm_exp_fail;

  char pgm_fn[11+MAX_PGM_FN_LEN+1];
  pgm_import_sel_t pgm_sel;

  strcpy(pgm_fn,PGM_DIR);
  strcat(pgm_fn,"/");

  res = select_programs("Select Programs", pgm_sel.pgm_indices, &pgm_sel.pgm_cnt);
  if ( res != 0 || pgm_sel.pgm_cnt == 0 )
    return; // FIXME: write something?
  
  //void file_selection_screen(const char * title, const char * base_dir, const char * ext, file_sel_fn_t sel_fn, int disp_new, void * data)
  file_selection_screen("Program Export Filename", PGM_DIR, PGM_EXT, pgm_export_enter, 1, 1, &pgm_sel); 

pgm_exp_fail:
  sys_disk_check_valid(); // Unmount drive if disk is invalid
  if ( !sys_disk_ok() ) {
    disp_disk_info("Program Export");
    wait_for_key_press();
  }
}










/*
 ▄    ▄                                                    ▄                 
 ██  ██  ▄▄▄   ▄ ▄▄   ▄   ▄          ▄▄▄   ▄   ▄   ▄▄▄   ▄▄█▄▄   ▄▄▄   ▄▄▄▄▄ 
 █ ██ █ █▀  █  █▀  █  █   █         █   ▀  ▀▄ ▄▀  █   ▀    █    █▀  █  █ █ █ 
 █ ▀▀ █ █▀▀▀▀  █   █  █   █          ▀▀▀▄   █▄█    ▀▀▀▄    █    █▀▀▀▀  █ █ █ 
 █    █ ▀█▄▄▀  █   █  ▀▄▄▀█         ▀▄▄▄▀   ▀█    ▀▄▄▄▀    ▀▄▄  ▀█▄▄▀  █ █ █ 
                                            ▄▀                               
                                           ▀▀                                */




const uint8_t mid_menu[] = {
    MI_FILE,
    MI_STATEFILE,
    MI_SETTINGS,
    MI_SYSTEM_ENTER,
    MI_ABOUT_PGM,
    0 }; // Terminator


const uint8_t mid_file[] = {
    MI_LOAD_PGM,
    MI_SAVE_PGM,
    MI_MSC,
    MI_DISK_INFO,
    0 }; // Terminator



const uint8_t mid_statefile[] = {
    MI_LOAD_STATE,
    MI_SAVE_STATE,
    MI_CLEAN_RESET,
    0 }; // Terminator




const uint8_t mid_settings[] = {
    MI_SET_TIME,
    MI_SET_DATE,
    MI_TOPBAR_MENU,
    MI_BEEP_MUTE,
    MI_SLOW_AUTOREP,
    MI_STACK_CONFIG,
    0 }; // Terminator


const uint8_t mid_topbar[] = {
    MI_DISP_STATFN,
    MI_DISP_DOW,
    MI_DISP_DATE,
    MI_DISP_DATE_SEP,
    MI_DISP_SHORTMON,
    MI_DISP_TIME,
    MI_DISP_VOLTAGE,
    0 }; // Terminator


const uint8_t mid_stack[] = {
    MI_STACK_XYZTL,
    MI_STACK_XYZTA,
    MI_STACK_XYZT,
    MI_STACK_XYL,
    MI_STACK_XYA,
    MI_STACK_XY,
    0 }; // Terminator




int stack_menu_index() {
  const uint8_t ixs[] = {
    STACK_XYZTL,
    STACK_XYZTA,
    STACK_XYZT,
    STACK_XYL,
    STACK_XYA,
    STACK_XY,
    0 }; // Terminator
  int ix = 0;
  int sl = get_stack_layout();

  for(ix=0; ixs[ix]; ix++)
    if (ixs[ix] == sl)
      return ix;

  return 0;
}



const smenu_t         MID_MENU = { "Setup",  mid_menu,   NULL, NULL };
const smenu_t         MID_FILE = { "File",   mid_file,   NULL, NULL };
const smenu_t     MID_SETTINGS = { "Settings",  mid_settings,  NULL, NULL};
const smenu_t MID_STACK_CONFIG = { "Stack Layout", mid_stack, NULL, NULL};
const smenu_t    MID_STATEFILE = { "Calculator State", mid_statefile, NULL, NULL};
const smenu_t       MID_TOPBAR = { "Status Bar", mid_topbar, NULL, NULL};


// ----------------------------------------------------------------------------------





// Prototypes




int run_menu_item(uint8_t line_id) {
  int ret = 0;

  switch(line_id) {
  //void file_selection_screen(const char * title, const char * base_dir, const char * ext, file_sel_fn_t sel_fn, int disp_new, void * data)
  case MI_LOAD_PGM:
    //pgm_import();
    ret = file_selection_screen("Program Import", PGM_DIR, PGM_EXT, pgm_import_enter, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;
  case MI_SAVE_PGM:
    pgm_export();
    break;
  case MI_LOAD_STATE:
    ret = file_selection_screen("Load Calculator State", STATE_DIR, STATE_EXT, load_statefile, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;
  case MI_SAVE_STATE:
    // Don't pass through if the power is insufficient  
    if ( power_check_screen() ) break;
    ret = file_selection_screen("Save Calculator State", STATE_DIR, STATE_EXT, save_statefile, 1, 1, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;

  case MI_CLEAN_RESET:
    run_reset_state_file();
    //ret = MRET_EXIT;
    break;

  //case MI_SAVE_STAT:
  //  ret = MRET_SAVESTATE;
  //  break;


  case MI_FILE:
    ret = handle_menu(&MID_FILE,MENU_ADD, 0);
    break;
  case MI_STATEFILE:
    ret = handle_menu(&MID_STATEFILE,MENU_ADD, 0);
    break;

  /* Stack */
  case MI_STACK_CONFIG:
    handle_menu(&MID_STACK_CONFIG,MENU_ADD,stack_menu_index());
    break;
  case MI_STACK_XYZTL:
    set_stack_layout(STACK_XYZTL);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYZTA:
    set_stack_layout(STACK_XYZTA);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYZT:
    set_stack_layout(STACK_XYZT);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYL:
    set_stack_layout(STACK_XYL);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYA:
    set_stack_layout(STACK_XYA);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XY:
    set_stack_layout(STACK_XY);
    ret = MRET_EXIT;
    break;

  /* Top Bar */
  case MI_TOPBAR_MENU:
    ret = handle_menu(&MID_TOPBAR, MENU_ADD, 0);
    break;
  case MI_DISP_STATFN:   toggle_disp(DISP_STATFN);   break;
  case MI_DISP_DOW:      toggle_disp(DISP_DOW);      break;
  case MI_DISP_DATE:     toggle_disp(DISP_DATE);     break;
  case MI_DISP_DATE_SEP: toggle_disp(DISP_DATE_SEP); break;
  case MI_DISP_SHORTMON: toggle_disp(DISP_SHORTMON); break;
  case MI_DISP_TIME:     toggle_disp(DISP_TIME);     break;
  case MI_DISP_VOLTAGE:  toggle_disp(DISP_VOLTAGE);  break;

  case MI_SETTINGS:
    ret = handle_menu(&MID_SETTINGS,MENU_ADD, 0);
    break;

  case MI_ABOUT_PGM:
    lcd_for_dm42(DISP_ABOUT);
    wait_for_key_press();
    break;

   default:
    ret = MRET_UNIMPL;
    break;
  }

  return ret;
}




char * sep_str(char * s, char const *txt, char c) {
  sprintf(s,"[ ] %s", txt);
  s[1] = c;
  return s;
}


char * orb_str(char * s, char const *txt, int val) {
  strcpy(s, rb_str(val));
  strcat(s, " ");
  strcat(s, txt);
  return s;
}


char * b_str(char * s, char const *txt, int val) {
  s[0]=0;
  if(val)
    strcpy(s, "\xff");
  strcat(s, txt);
  return s;
}




char * layout_str(char * s, const char * txt) {
  sprintf(s, "%s  [ ", txt);
  get_stack_layout_str(s+strlen(s), get_stack_layout());
  strcat(s, " ]");
  return s;
}


char * state_str(char *s, const char * txt) {
  const int maxlen = 30;
  sprintf(s, "%s [", txt);
  copy_reset_state_filename(s,maxlen);
  strcat(s, "]");
  return s;
}


// Returns NULL if not found
const char * menu_line_str(uint8_t line_id, char * s, const int slen) {
  const char * ln;

  switch(line_id) {
  case MI_LOAD_PGM:     ln = "Load Program >";       break;
  case MI_SAVE_PGM:     ln = "Save Program >";       break;
  case MI_SAVE_STAT:    ln = "Save Calculator State";break;
  case MI_FILE:         ln = "File >";               break;
  case MI_STATEFILE:    ln = state_str(s, "Calc. State >"); break;
  case MI_LOAD_STATE:   ln = "Load State";           break;
  case MI_SAVE_STATE:   ln = "Save State";           break;
  case MI_CLEAN_RESET:  ln = "Load Clean State";     break;

  case MI_STACK_CONFIG:
    ln = layout_str(s, "Stack Layout");              break;

  case MI_STACK_XYZTL:  ln = orb_str(s, "XYZTL", get_stack_layout() == STACK_XYZTL); break;
  case MI_STACK_XYZTA:  ln = orb_str(s, "XYZTA", get_stack_layout() == STACK_XYZTA); break;
  case MI_STACK_XYZT:   ln = orb_str(s, "XYZT",  get_stack_layout() == STACK_XYZT);  break;
  case MI_STACK_XYL:    ln = orb_str(s, "XYL",   get_stack_layout() == STACK_XYL);   break;
  case MI_STACK_XYA:    ln = orb_str(s, "XYA",   get_stack_layout() == STACK_XYA);   break;
  case MI_STACK_XY:     ln = orb_str(s, "XY",    get_stack_layout() == STACK_XY);    break;

  case MI_TOPBAR_MENU:  ln = "Status Bar >";         break;
  case MI_DISP_STATFN:  ln = opt_str(s, " State Filename",  is_disp(DISP_STATFN));  break;
  case MI_DISP_DOW:     ln = opt_str(s, " Day of Week",     is_disp(DISP_DOW));     break;
  case MI_DISP_DATE:    ln = opt_str(s, " Date",            is_disp(DISP_DATE));    break;
  case MI_DISP_DATE_SEP:ln = sep_str(s, " Date Separator",  get_disp_date_sep());   break;
  case MI_DISP_SHORTMON:ln = opt_str(s, "  Month Shortcut", is_disp(DISP_SHORTMON));break;
  case MI_DISP_TIME:    ln = opt_str(s, " Time",            is_disp(DISP_TIME));    break;
  case MI_DISP_VOLTAGE: 
    ln = opt_str(s, " Power Voltage in Header", is_disp(DISP_VOLTAGE));   break;

  case MI_SETTINGS:     ln = "Settings >";           break;
  case MI_ABOUT_PGM:    ln = "About >";              break;

  default:
    ln = NULL;
    break;
  }

  return ln;
}


// -------------------------------------------------------------------



} // C
