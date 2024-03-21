#include <iostream>
#include <boost/process.hpp>
#include <string.h>
#include <xcb/xcb.h>


void get_children(xcb_connection_t* dpy, xcb_window_t window, xcb_window_t** children, int* count) {
  *count = 0;
  *children = NULL;
  auto cookie = xcb_query_tree(dpy, window);
  auto reply = xcb_query_tree_reply(dpy, cookie, NULL);
  if (reply) {
    *count = xcb_query_tree_children_length(reply);
    *children = xcb_query_tree_children(reply);
    free(reply);
  }
}

void get_class_name(xcb_connection_t* dpy, xcb_window_t window, char** name, int* length) {
  *length = 0;
  *name = NULL;
  auto cookie = xcb_get_property(dpy, 0, window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 0, 1000);
  auto reply = xcb_get_property_reply(dpy, cookie, NULL);
  if (reply) {
    *length = xcb_get_property_value_length(reply);
    *name = (char*)xcb_get_property_value(reply);
  }
}

xcb_window_t get_scratchpad_id(xcb_connection_t *dpy) {
  auto screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
  int nChildren;
  xcb_window_t* children;
  get_children(dpy, screen->root, &children, &nChildren);
  for (int i = 0; i < nChildren; i++) {
    xcb_window_t wid = children[i];
    int length;
    char* name;
    get_class_name(dpy, wid, &name, &length);
    if (strcmp(name, "scratchpad") == 0) {
      return wid;
    }
  }
  return 0;
}

void open_or_close_scratchpad() {
  auto dpy = xcb_connect(":0.0", NULL);
  xcb_window_t wid = 0;
  wid = get_scratchpad_id(dpy);
  if (static_cast<int>(wid) != 0) {
    xcb_get_window_attributes_reply_t *attrs = xcb_get_window_attributes_reply (dpy, xcb_get_window_attributes(dpy, wid), NULL);
    uint8_t map_status = attrs->map_state;
    if (static_cast<int>(map_status) == 0) {
      // scratchpad is hidden
      xcb_map_window(dpy, wid);
    } else {
      // scratchpad is opened
      xcb_unmap_window(dpy, wid);
    }
  } else {
    // scratchpad is closed
    namespace bp = boost::process;
    /* bp::spawn detaches process */
    bp::spawn("/usr/bin/alacritty", "--class", "scratchpad");
  }
  xcb_flush(dpy);
}

int main() {
  open_or_close_scratchpad();
  return 0;
}
