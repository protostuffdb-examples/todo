<template>
<div>
  <div id="popup" class="popup">&nbsp;</div>
  <!--<div style="display:none">
    <Suggest />
    <Calendar />
  </div>-->
  <div class="container-full-width">
    <home></home>
  </div>
</div>
</template>
<script>
import * as ui from './ui'
//import Suggest from 'coreds-ui/lib/c/suggest'
//import Calendar from 'coreds-ui/lib/c/calendar'
import Home from './user/'
export default {
  data() {
    return {}
  },
  components: {
    //Suggest,
    //Calendar,
    Home
  }
}
</script>
